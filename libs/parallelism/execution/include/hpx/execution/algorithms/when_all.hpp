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
        template <typename... Ts>
        struct when_all_state
        {
            std::atomic<std::size_t> predecessors_remaining;
            hpx::util::member_pack_for<std::decay_t<Ts>...> ts;
            std::atomic<bool> set_done_error_called{false};
        };

        template <typename R, std::size_t I, typename... Ts>
        struct when_all_receiver
        {
            std::shared_ptr<when_all_state<Ts...>> st;
            typename std::decay<R>::type r;

            template <typename R_>
            when_all_receiver(std::shared_ptr<when_all_state<Ts...>> st, R_&& r)
              : st(st)
              , r(std::forward<R>(r))
            {
            }

            template <typename E>
            void set_error(E&& e) noexcept
            {
                // All set_value calls are ignored once set_done is called.
                // TODO: Wait for all errors? error_types is only a variant,
                // i.e. set_error should only be called with one value. Should
                // the errors need to be combined or is the first one enough?
                if (!st->set_done_error_called.exchange(true))
                {
                    hpx::execution::experimental::set_error(
                        std::move(r), std::forward<E>(e));
                }
            }

            void set_done() noexcept
            {
                // All set_value calls are ignored once set_error is called.
                if (!st->set_done_error_called.exchange(true))
                {
                    hpx::execution::experimental::set_done(std::move(r));
                }
            };

            template <std::size_t... Is>
            void set_value_helper(
                hpx::util::member_pack<hpx::util::index_pack<Is...>, Ts...>& ts)
            {
                hpx::execution::experimental::set_value(
                    std::move(r), std::move(ts.template get<Is>())...);
            }

            template <typename T>
            void set_value(T&& t) noexcept
            {
                st->ts.template get<I>() = std::forward<T>(t);

                if (--st->predecessors_remaining == 0 &&
                    !st->set_done_error_called)
                {
                    set_value_helper(st->ts);
                }
            }
        };

        template <typename... Ss>
        struct when_all_sender
        {
            hpx::util::member_pack_for<std::decay_t<Ss>...> senders;

            template <typename... Ss_>
            explicit constexpr when_all_sender(Ss_&&... ss)
              : senders(std::piecewise_construct, std::forward<Ss_>(ss)...)
            {
            }

            template <typename Sender>
            struct value_types_helper
            {
                using value_types =
                    typename hpx::execution::experimental::sender_traits<
                        Sender>::template value_types<hpx::util::pack,
                        hpx::util::pack>;
                using type =
                    typename detail::when_all_single_result<value_types>::type;
            };

            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types =
                Variant<Tuple<typename value_types_helper<Ss>::type...>>;

            template <template <typename...> class Variant>
            using error_types = typename hpx::util::detail::unique<
                typename hpx::util::detail::cat<
                    typename hpx::execution::experimental::sender_traits<
                        Ss>::template error_types<Variant>...>::type>::type;

            static constexpr bool sends_done = false;

            static constexpr std::size_t num_predecessors = sizeof...(Ss);

            template <typename OperationStates>
            struct operation_state;

            template <std::size_t... Is, typename... Os>
            struct operation_state<
                hpx::util::member_pack<hpx::util::index_pack<Is...>, Os...>>
            {
                hpx::util::member_pack<hpx::util::index_pack<Is...>, Os...>
                    operation_states;

                void start() noexcept
                {
                    int sequencer[] = {
                        (hpx::execution::experimental::start(
                             operation_states.template get<Is>()),
                            0)...};
                    (void) sequencer;
                }
            };

            template <typename... Os>
            auto make_operation_state(Os&&... os)
            {
                return operation_state<hpx::util::member_pack_for<Os...>>{
                    hpx::util::member_pack<typename hpx::util::make_index_pack<
                                               sizeof...(Os)>::type,
                        Os...>(
                        std::piecewise_construct, std::forward<Os>(os)...)};
            }

            using state_type =
                when_all_state<typename value_types_helper<Ss>::type...>;
            std::shared_ptr<state_type> st = std::make_shared<state_type>();

            template <typename>
            struct check_type;

            template <typename R, std::size_t... Is, typename... Ss_>
            auto connect_helper(R&& r, std::shared_ptr<state_type>&& st,
                hpx::util::member_pack<hpx::util::index_pack<Is...>, Ss_...>&&
                    senders)
            {
                st->predecessors_remaining = num_predecessors;

                return make_operation_state(
                    (hpx::execution::experimental::connect(
                        std::move(senders.template get<Is>()),
                        when_all_receiver<R, Is,
                            typename value_types_helper<Ss_>::type...>(
                            st, r)))...);
            }

            template <typename R>
            auto connect(R&& r)
            {
                return connect_helper(
                    std::forward<R>(r), std::move(st), std::move(senders));
            }
        };
    }    // namespace detail

    HPX_INLINE_CONSTEXPR_VARIABLE struct when_all_t final
      : hpx::functional::tag_fallback<when_all_t>
    {
    private:
        template <typename... Ss>
        friend constexpr HPX_FORCEINLINE auto tag_fallback_invoke(
            when_all_t, Ss&&... ss)
        {
            static_assert(sizeof...(Ss) >= 0,
                "when_all expects at least one predecessor sender");
            return detail::when_all_sender<Ss...>{std::forward<Ss>(ss)...};
        }
    } when_all{};
}}}    // namespace hpx::execution::experimental
