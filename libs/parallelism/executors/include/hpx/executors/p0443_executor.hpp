//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/coroutines/thread_enums.hpp>
#include <hpx/execution/detail/post_policy_dispatch.hpp>
#include <hpx/execution/executors/execution_parameters.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/execution_base/sender.hpp>

#include <cstddef>
#include <exception>
#include <type_traits>
#include <utility>

namespace hpx { namespace execution { namespace experimental {
    struct executor
    {
        constexpr executor() = default;

        /// \cond NOINTERNAL
        bool operator==(executor const& rhs) const noexcept
        {
            return pool_ == rhs.pool_ && priority_ == rhs.priority_ &&
                stacksize_ == rhs.stacksize_ &&
                schedulehint_ == rhs.schedulehint_;
        }

        bool operator!=(executor const& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        friend executor tag_invoke(
            hpx::execution::experimental::make_with_priority_t,
            executor const& exec, hpx::threads::thread_priority priority)
        {
            auto exec_with_priority = exec;
            exec_with_priority.priority_ = priority;
            return exec_with_priority;
        }

        friend hpx::threads::thread_priority tag_invoke(
            hpx::execution::experimental::get_priority_t, executor const& exec)
        {
            return exec.priority_;
        }

        friend executor tag_invoke(
            hpx::execution::experimental::make_with_stacksize_t,
            executor const& exec, hpx::threads::thread_stacksize stacksize)
        {
            auto exec_with_stacksize = exec;
            exec_with_stacksize.stacksize_ = stacksize;
            return exec_with_stacksize;
        }

        friend hpx::threads::thread_stacksize tag_invoke(
            hpx::execution::experimental::get_stacksize_t, executor const& exec)
        {
            return exec.stacksize_;
        }

        friend executor tag_invoke(
            hpx::execution::experimental::make_with_hint_t,
            executor const& exec, hpx::threads::thread_schedule_hint hint)
        {
            auto exec_with_hint = exec;
            exec_with_hint.schedulehint_ = hint;
            return exec_with_hint;
        }

        friend hpx::threads::thread_schedule_hint tag_invoke(
            hpx::execution::experimental::get_hint_t, executor const& exec)
        {
            return exec.schedulehint_;
        }

        template <typename F>
        void execute(F&& f) const
        {
            hpx::util::thread_description desc(f);

            hpx::parallel::execution::detail::post_policy_dispatch<
                hpx::launch::async_policy>::call(hpx::launch::async, desc,
                pool_, priority_, stacksize_, schedulehint_,
                std::forward<F>(f));
        }

        template <typename Executor, typename R>
        struct operation_state
        {
            std::decay_t<Executor> exec;
            std::decay_t<R> r;

            void start() noexcept
            {
                try
                {
                    hpx::execution::experimental::execute(
                        exec, [r = std::move(r)]() mutable {
                            hpx::execution::experimental::set_value(
                                std::move(r));
                        });
                }
                catch (...)
                {
                    hpx::execution::experimental::set_error(
                        std::move(r), std::current_exception());
                }
            }
        };

        template <typename Executor>
        struct sender
        {
            std::decay_t<Executor> exec;

            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = Variant<Tuple<>>;

            template <template <typename...> class Variant>
            using error_types = Variant<std::exception_ptr>;

            static constexpr bool sends_done = false;

            template <typename R>
            operation_state<Executor, R> connect(R&& r) &&
            {
                return {std::move(exec), std::forward<R>(r)};
            }
        };

        template <template <class...> class Tuple,
            template <class...> class Variant>
        using value_types = Variant<Tuple<>>;

        template <template <class...> class Variant>
        using error_types = Variant<std::exception_ptr>;

        static constexpr bool sends_done = false;

        template <typename R>
        operation_state<executor, R> connect(R&& r) &&
        {
            return {*this, std::forward<R>(r)};
        }

        constexpr sender<executor> schedule() const
        {
            return {*this};
        }
        /// \endcond

    private:
        /// \cond NOINTERNAL
        hpx::threads::thread_pool_base* pool_ =
            hpx::threads::detail::get_self_or_default_pool();
        hpx::threads::thread_priority priority_ =
            hpx::threads::thread_priority::normal;
        hpx::threads::thread_stacksize stacksize_ =
            hpx::threads::thread_stacksize::small_;
        hpx::threads::thread_schedule_hint schedulehint_{};
        /// \endcond
    };

    // TODO: Very experimental.
    struct stream_executor
    {
        constexpr stream_executor() = default;

        /// \cond NOINTERNAL
        bool operator==(stream_executor const& rhs) const noexcept
        {
            return stream_ == rhs.stream_;
        }

        bool operator!=(stream_executor const& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        // TODO: This could set the stream priority.
        // friend stream_executor tag_invoke(
        //     hpx::execution::experimental::make_with_priority_t,
        //     stream_executor const& exec, hpx::threads::thread_priority priority)
        // {
        //     auto exec_with_priority = exec;
        //     exec_with_priority.priority_ = priority;
        //     return exec_with_priority;
        // }

        template <typename F>
        void execute(F&& f) const
        {
            HPX_INVOKE(f, stream_);
        }

        template <typename R>
        struct operation_state
        {
            typename std::decay<R>::type r;

            void start() noexcept
            {
                // TODO: What does this need to do? It could do nothing. Can
                // work be submitted eagerly? It does need to honour the receiver though.

                // Record an event and start a task which will `set_value` when
                // the event is ready?

                // Store the active stream?

                // Store the event?

                // NOTE: This triggers the receivers. Where and when should an
                // event be inserted? Where and when should the event be waited
                // on.
                hpx::execution::experimental::set_value(std::move(r));
            }
        };

        struct sender
        {
            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = Variant<Tuple<>>;

            template <template <typename...> class Variant>
            using error_types = Variant<std::exception_ptr>;

            static constexpr bool sends_done = false;

            template <typename R>
            operation_state<R> connect(R&& r)
            {
                return {std::forward<R>(r)};
            }

            friend constexpr HPX_FORCEINLINE void tag_invoke(
                sync_wait_t, sender s)
            {
                // Nothing has been scheduled yet so there is nothing to wait
                // for. Return immediately.
            }
        };

        template <template <class...> class Tuple,
            template <class...> class Variant>
        using value_types = Variant<Tuple<>>;

        template <template <class...> class Variant>
        using error_types = Variant<std::exception_ptr>;

        static constexpr bool sends_done = false;

        template <typename R>
        operation_state<R> connect(R&& r)
        {
            return {std::forward<R>(r)};
        }

        sender schedule()
        {
            return {};
        }
        /// \endcond

    private:
        /// \cond NOINTERNAL
        cudaStream_t stream_{};
        /// \endcond
    };

    struct stream_transform_sender
    {
        template <template <typename...> class Tuple,
            template <typename...> class Variant>
        using value_types = Variant<Tuple<>>;

        template <template <typename...> class Variant>
        using error_types = Variant<std::exception_ptr>;

        static constexpr bool sends_done = false;

        template <typename R>
        operation_state<R> connect(R&& r)
        {
            return {std::forward<R>(r)};
        }

        friend constexpr HPX_FORCEINLINE void tag_invoke(
            sync_wait_t, stream_transform_sender s)
        {
            // Something has been scheduled. Block until event is ready.
            // TODO: Use scheduler integration.
            hpx::util::yield_while([this]() { return this->event_ == ready; });
        }
    };

    template <typename F>
    HPX_FORCEINLINE auto tag_invoke(
        transform_t, stream_executor::sender s, F&& f)
    {
        // Eagerly or lazily trigger submission?

        // Let's try eagerly first. The predecessor is known to be a
        // void sender.
        HPX_INVOKE(f, stream_);

        // Need to return a sender which, when waited on, completes only
        // when the event is done.

        return
    }
}}}    // namespace hpx::execution::experimental
