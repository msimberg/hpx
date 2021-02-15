//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/execution/algorithms/detail/single_result.hpp>
#include <hpx/execution_base/operation_state.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/execution_base/sender.hpp>
#include <hpx/functional/tag_fallback_invoke.hpp>
#include <hpx/synchronization/mutex.hpp>
#include <hpx/type_support/pack.hpp>

#include <atomic>
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace hpx { namespace execution { namespace experimental {
    namespace detail {
        struct fork_state
        {
            hpx::lcos::local::mutex mtx;
            bool started = false;
            bool result_ready = false;
            std::vector<std::function<void()>> continuations;
        };

        struct fork_receiver
        {
            std::shared_ptr<fork_state> st;

            fork_receiver(std::shared_ptr<fork_state> st)
              : st(st)
            {
            }

            template <typename E>
            void set_error(E&&) noexcept
            {
                // TODO
            }

            void set_done() noexcept {
                // TODO
            };

            template <typename... Ts>
            void set_value(Ts&&... ts) noexcept
            {
                // TODO: Too coarse.
                std::lock_guard<hpx::lcos::local::mutex> l{st->mtx};
                st->result_ready = true;

                for (auto f : st->continuations)
                {
                    f();
                }
            }
        };

        template <typename S, typename Scheduler>
        struct fork_sender
        {
            typename std::decay<S>::type s;
            typename std::decay<Scheduler>::type scheduler;

            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types =
                typename hpx::execution::experimental::sender_traits<
                    S>::template value_types<Tuple, Variant>;

            template <template <typename...> class Variant>
            using error_types =
                typename hpx::execution::experimental::sender_traits<
                    S>::template error_types<Variant>;

            static constexpr bool sends_done = false;

            template <typename R>
            struct operation_state
            {
                std::shared_ptr<fork_state> st;
                typename std::decay<Scheduler>::type scheduler;
                typename std::decay<R>::type r;

                void start() noexcept
                {
                    auto f = [scheduler = std::move(scheduler),
                                 r = std::move(r)]() {
                        // TODO: Error handling?
                        // TODO: Should this use schedule + connect + start?
                        // TODO: This could be inline for the first/last
                        // continuation? If the scheduler is the same?
                        hpx::execution::experimental::execute(
                            scheduler, [r = std::move(r)]() mutable {
                                hpx::execution::experimental::set_value(
                                    std::move(r));
                            });
                    };

                    // TODO: This may be way too coarse.
                    std::lock_guard<hpx::lcos::local::mutex> l{st->mtx};
                    if (st->result_ready)
                    {
                        // If the result is already available we trigger the
                        // continuation immediately.
                        f();
                    }
                    else
                    {
                        // If the result is not available we add it to the
                        // shared state which will be triggered when the
                        // predecessor is ready.
                        st->continuations.push_back(std::move(f));
                    }
                }
            };

            std::shared_ptr<fork_state> st = std::make_shared<fork_state>();

            // TODO: Make this work for non-void functions.
            template <typename R>
            auto connect(R&& r)
            {
                // Trigger senders until this point to start.
                // TODO: This is way to coarse. Do we even need a mutex or could
                // this be done with atomics/lockfree queue?
                std::lock_guard<hpx::lcos::local::mutex> l{st->mtx};
                if (!st->started)
                {
                    st->started = true;
                    hpx::execution::experimental::start(
                        hpx::execution::experimental::connect(
                            s, fork_receiver(st)));
                    // TODO: Passing r to fork_receiver did not work last time I
                    // tried. What goes wrong?
                }

                // Then return a new operation state which triggers the
                // continuations when started.
                return operation_state<R>{st, scheduler, std::forward<R>(r)};
            }
        };
    }    // namespace detail

    HPX_INLINE_CONSTEXPR_VARIABLE struct fork_t final
      : hpx::functional::tag_fallback<fork_t>
    {
    private:
        // TODO: How generic is this implementation?
        template <typename S, typename Scheduler>
        friend constexpr HPX_FORCEINLINE auto tag_fallback_invoke(
            fork_t, S&& s, Scheduler&& scheduler)
        {
            return detail::fork_sender<S, Scheduler>{
                std::forward<S>(s), std::forward<Scheduler>(scheduler)};
        }
    } fork{};
}}}    // namespace hpx::execution::experimental
