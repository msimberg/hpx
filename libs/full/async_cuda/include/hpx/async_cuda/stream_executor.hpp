//  Copyright (c) 2021 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/async_cuda/custom_gpu_api.hpp>
#include <hpx/execution/algorithms/on.hpp>
#include <hpx/execution/algorithms/transform.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/execution_base/sender.hpp>
#include <hpx/execution_base/this_thread.hpp>

#include <exception>
#include <type_traits>
#include <utility>

// This file defines a stream_executor and associated specializations for
// execution algorithms where required.
//
// All specializations currently require void senders, i.e. no values are
// propagated between calls.
//
// The following algorithms are meant to just work:
// - schedule
// - on
// - transform
// - sync_wait
//
// The following algorithms should have reasonable default behaviour, but are
// untested and may only work partially or not at all.
// - fork (untested)
// - just (only for the degenerate void case)
// - just_on (untested)
// - let_error (untested)
// - let_value (untested)
// - when_all (untested)
//
// Potential specializations:
// - any stream sender to sync_wait: could have the event callback signal a
//   condition variable instead of using yield_while.
// - stream senders with no work (on, schedule) to sync_wait: no-op.
//
// To do:
// - Optimize waiting for stream.
// - Optimize certain degenerate cases.
// - Thoroughly test.
// - Add some kind of stream pool integration?
namespace hpx { namespace cuda { namespace experimental {
    namespace detail {
        void wait_stream(cudaStream_t stream)
        {
            cudaEvent_t event;

            cudaError_t error =
                cudaEventCreateWithFlags(&event, cudaEventDisableTiming);
            if (error != cudaSuccess)
            {
                std::terminate();
            }

            error = cudaEventRecord(event, stream);
            if (error != cudaSuccess)
            {
                std::terminate();
            }

            hpx::util::yield_while([&] {
                error = cudaEventQuery(event);
                return error == cudaErrorNotReady;
            });

            if (error != cudaSuccess)
            {
                std::terminate();
            }

            error = cudaEventDestroy(event);
            if (error != cudaSuccess)
            {
                std::terminate();
            }
        }

        template <typename S, typename Enable = void>
        struct is_stream_sender : std::false_type
        {
        };

        // TODO: Base this on a member function instead of variable? Property?
        template <typename S>
        struct is_stream_sender<S,
            std::void_t<decltype(std::declval<S>().stream)>> : std::true_type
        {
        };

        template <typename R>
        struct stream_executor_operation_state
        {
            typename std::decay<R>::type r;

            // Stay on the context of the caller.
            void start() noexcept
            {
                hpx::execution::experimental::set_value(std::move(r));
            }
        };

        struct stream_executor_sender
        {
            cudaStream_t stream;

            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = Variant<Tuple<>>;

            template <template <typename...> class Variant>
            using error_types = Variant<std::exception_ptr>;

            static constexpr bool sends_done = false;

            template <typename R>
            stream_executor_operation_state<R> connect(R&& r)
            {
                return {std::forward<R>(r)};
            }
        };
    }    // namespace detail

    struct stream_executor
    {
        constexpr stream_executor() = default;

        stream_executor(cudaStream_t stream)
          : stream(stream)
        {
        }

        bool operator==(stream_executor const& rhs) const noexcept
        {
            return stream == rhs.stream;
        }

        bool operator!=(stream_executor const& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        template <typename F>
        void execute(F&& f) const
        {
            HPX_INVOKE(f, stream);
        }

        template <template <class...> class Tuple,
            template <class...> class Variant>
        using value_types = Variant<Tuple<>>;

        template <template <class...> class Variant>
        using error_types = Variant<std::exception_ptr>;

        static constexpr bool sends_done = false;

        template <typename R>
        detail::stream_executor_operation_state<R> connect(R&& r)
        {
            return {std::forward<R>(r)};
        }

        detail::stream_executor_sender schedule()
        {
            return {stream};
        }

        cudaStream_t stream{};
    };

    namespace detail {
        template <typename R, typename F>
        struct stream_transform_receiver;

        template <typename R>
        struct is_stream_transform_receiver : std::false_type
        {
        };

        template <typename R, typename F>
        struct is_stream_transform_receiver<stream_transform_receiver<R, F>>
          : std::true_type
        {
        };

        template <typename R, typename F>
        struct stream_transform_receiver
        {
            typename std::decay<R>::type r;
            typename std::decay<F>::type f;
            cudaStream_t stream;

            template <typename R_, typename F_>
            stream_transform_receiver(R_&& r, F_&& f, cudaStream_t stream)
              : r(std::forward<R>(r))
              , f(std::forward<F>(f))
              , stream(stream)
            {
            }

            template <typename E>
            void set_error(E&& e) noexcept
            {
                hpx::execution::experimental::set_error(
                    std::move(r), std::forward<E>(e));
            }

            void set_done() noexcept
            {
                hpx::execution::experimental::set_done(std::move(r));
            };

            void set_value() noexcept
            {
                std::cerr << "spawning f on stream " << stream << std::endl;
                try
                {
                    HPX_INVOKE(f, stream);
                }
                catch (...)
                {
                    hpx::execution::experimental::set_error(
                        std::move(r), std::current_exception());
                }

                // If we know that the successor receiver is also a
                // stream_transform_receiver we can skip the wait here and let the
                // successor synchronize instead. This misses some degenerate
                // chaining optimization opportunities, such as:
                //     s1 = schedule(stream_executor);
                //     s2 = transform(s1, f);
                //     // Unnecessarily block after f even if the executors use the
                //     // same stream
                //     s3 = on(s2, stream_executor);
                //     s4 = transform(s3, f);
                //     sync_wait(s4);
                if constexpr (!detail::is_stream_transform_receiver<
                                  std::decay_t<R>>::value)
                {
                    detail::wait_stream(stream);
                }

                hpx::execution::experimental::set_value(std::move(r));
            }
        };

        template <typename S, typename F>
        struct stream_transform_sender
        {
            typename std::decay<S>::type s;
            typename std::decay<F>::type f;
            cudaStream_t stream = s.stream;

            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = Variant<Tuple<>>;

            template <template <typename...> class Variant>
            using error_types = Variant<std::exception_ptr>;

            static constexpr bool sends_done = false;

            template <typename R>
            auto connect(R&& r)
            {
                return hpx::execution::experimental::connect(std::move(s),
                    stream_transform_receiver<R, F>(
                        std::forward<R>(r), std::move(f), s.stream));
            }
        };

        // f will be called with an additional stream argument. This
        // specialization only applies when the predecessor is a "stream
        // sender", i.e. one that carries a stream member.
        template <typename S, typename F,
            typename Enable =
                std::enable_if_t<detail::is_stream_sender<S>::value>>
        auto tag_invoke(hpx::execution::experimental::transform_t, S&& s, F&& f)
        {
            return stream_transform_sender<S, F>{
                std::forward<S>(s), std::forward<F>(f)};
        }

        template <typename R>
        struct stream_on_receiver
        {
            typename std::decay<R>::type r;
            stream_executor scheduler;

            template <typename R_>
            stream_on_receiver(R_&& r, stream_executor scheduler)
              : r(std::forward<R>(r))
              , scheduler(std::move(scheduler))
            {
            }

            template <typename E>
            void set_error(E&& e) noexcept
            {
                hpx::execution::experimental::set_error(
                    std::move(r), std::forward<E>(e));
            }

            void set_done() noexcept
            {
                hpx::execution::experimental::set_done(std::move(r));
            };

            void set_value() noexcept
            {
                hpx::execution::experimental::set_value(std::move(r));
            }
        };

        template <typename S>
        struct stream_on_sender
        {
            typename std::decay<S>::type s;
            stream_executor scheduler;
            cudaStream_t stream = scheduler.stream;

            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = Variant<Tuple<>>;

            template <template <typename...> class Variant>
            using error_types = Variant<std::exception_ptr>;

            static constexpr bool sends_done = false;

            template <typename R>
            auto connect(R&& r)
            {
                return hpx::execution::experimental::connect(std::move(s),
                    stream_on_receiver<R>(
                        std::forward<R>(r), std::move(scheduler)));
            }
        };
    }    // namespace detail

    // Transition from the context of some sender s to a context where functions
    // are called with an additional stream argument. The actual running context
    // is kept the same (std::thread, hpx::thread, etc.).
    template <typename S>
    auto tag_invoke(
        hpx::execution::experimental::on_t, S&& s, stream_executor scheduler)
    {
        return detail::stream_on_sender<S>{
            std::forward<S>(s), std::move(scheduler)};
    }
}}}      // namespace hpx::cuda::experimental
