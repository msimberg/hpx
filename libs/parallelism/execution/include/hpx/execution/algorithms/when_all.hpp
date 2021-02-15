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
#include <hpx/functional/invoke_fused.hpp>
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
        struct when_all_state
        {
            std::atomic<std::size_t> predecessors_remaining;
        };

        template <typename R>
        struct when_all_receiver
        {
            std::shared_ptr<when_all_state> st;
            typename std::decay<R>::type r;

            template <typename R_>
            when_all_receiver(std::shared_ptr<when_all_state> st, R_&& r)
              : st(st)
              , r(std::forward<R>(r))
            {
            }

            template <typename E>
            void set_error(E&& e) noexcept
            {
                // TODO: This is most likely not right (no handling of multiple
                // attached predecessors).
                hpx::execution::experimental::set_error(
                    std::move(r), std::forward<E>(e));
            }

            void set_done() noexcept
            {
                // TODO: This is most likely not right (no handling of multiple
                // attached predecessors).
                hpx::execution::experimental::set_done(std::move(r));
            };

            template <typename... Ts>
            void set_value(Ts&&... ts) noexcept
            {
                // TODO: Too coarse.
                if (--st->predecessors_remaining == 0)
                {
                    hpx::execution::experimental::set_value(std::move(r));
                }
            }
        };

        template <typename... Ss>
        struct when_all_sender
        {
            std::tuple<std::decay_t<Ss>...> senders;

            // TODO
            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = Variant<Tuple<>>;

            // TODO
            template <template <typename...> class Variant>
            using error_types = Variant<>;

            static constexpr bool sends_done = false;

            template <typename OperationStates>
            struct operation_state
            {
                std::decay_t<OperationStates> operation_states;

                void start() noexcept
                {
                    hpx::util::invoke_fused(
                        [](auto... o) {
                            (...,
                                hpx::execution::experimental::start(
                                    std::move(o)));
                        },
                        operation_states);
                }
            };

            std::shared_ptr<when_all_state> st =
                std::make_shared<when_all_state>();

            // TODO: Make this work for non-void functions.
            // TODO: P1897 specifies that when_all is only for *non-void*
            // senders!
            template <typename R>
            auto connect(R&& r)
            {
                st->predecessors_remaining = sizeof...(Ss);
                auto operation_states = hpx::util::invoke_fused(
                    [this, &r](auto... s) {
                        return std::make_tuple(
                            hpx::execution::experimental::connect(
                                std::move(s), when_all_receiver<R>(st, r))...);
                    },
                    std::move(senders));
                return operation_state<decltype(operation_states)>{
                    std::move(operation_states)};
            }
        };
    }    // namespace detail

    HPX_INLINE_CONSTEXPR_VARIABLE struct when_all_t final
      : hpx::functional::tag_fallback<when_all_t>
    {
    private:
        // TODO: How generic is this implementation?
        template <typename... Ss>
        friend constexpr HPX_FORCEINLINE auto tag_fallback_invoke(
            when_all_t, Ss&&... ss)
        {
            return detail::when_all_sender<Ss...>{
                std::make_tuple(std::forward<Ss>(ss)...)};
        }
    } when_all{};
}}}    // namespace hpx::execution::experimental
