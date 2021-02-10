//  Copyright (c) 2021 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/execution_base/sender.hpp>
#include <hpx/functional/invoke_result.hpp>
#include <hpx/type_support/pack.hpp>

#include <exception>
// #include <optional>
// #include <tuple>
#include <type_traits>
#include <utility>
// #include <variant>

namespace hpx { namespace execution { namespace experimental {
    namespace detail {
        // TODO: Are the lifetimes of the arguments from s handled correctly?
        // Currently nothing is done to extend their lifetimes. Commented out
        // code refers to handling the arguments.
        template <typename R, typename F /*, typename Results*/>
        struct let_value_receiver
        {
            std::decay_t<R> r;    // OutputReceiver
            std::decay_t<F> f;    // SuccessorFactory

            // std::optional<Results> result = std::nullopt;

            template <typename R_, typename F_>
            let_value_receiver(R_&& r, F_&& f)
              : r(std::forward<R_>(r))
              , f(std::forward<F_>(f))
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

            template <typename... Ts>
            void set_value(Ts&&... ts) noexcept
            {
                try
                {
                    // TODO: Do the arguments live long enough?
                    // NOTE: Arguments are not forwarded. They should be taken
                    // by reference in f.
                    hpx::execution::experimental::start(
                        hpx::execution::experimental::connect(
                            HPX_INVOKE(f, ts...), r));
                }
                catch (...)
                {
                    hpx::execution::experimental::set_error(
                        r, std::current_exception());
                }
            }
        };

        template <typename S, typename R, typename F /*, typename Results*/>
        struct let_value_operation_state
        {
            using operation_state_type =
                std::decay_t<typename hpx::util::invoke_result<
                    hpx::execution::experimental::connect_t, S&&,
                    let_value_receiver<R, F /*, Results*/>>::type>;
            operation_state_type os;

            template <typename S_, typename R_, typename F_>
            let_value_operation_state(S_&& s, R_&& r, F_&& f)
              : os{hpx::execution::experimental::connect(std::move(s),
                    let_value_receiver<R, F /*, Results*/>(
                        std::forward<R_>(r), std::forward<F_>(f)))}
            {
            }

            void start() noexcept
            {
                hpx::execution::experimental::start(os);
            }
        };

        template <typename S, typename F>
        struct let_value_sender
        {
            typename std::decay<S>::type s;    // Predecessor
            typename std::decay<F>::type f;    // SuccessorFactory

            // template <template <typename...> class Tuple,
            //     template <typename...> class Variant>
            // using value_types_predecessor =
            //     typename hpx::execution::experimental::sender_traits<
            //         S>::template value_types<Tuple, Variant>;
            // using value_types_predecessor_concrete =
            //     value_types_predecessor<std::tuple, std::variant>;

            // TODO: Be generic. This should be the value_types of the sender
            // returned from f.
            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = Variant<Tuple<int>>;

            template <template <typename...> class Variant>
            using error_types =
                typename hpx::execution::experimental::sender_traits<
                    S>::template error_types<Variant>;

            static constexpr bool sends_done = false;

            template <typename R>
            auto connect(R&& r)
            {
                return let_value_operation_state<S, R,
                    F /*, value_types_predecessor_concrete*/>{
                    std::move(s), std::move(r), std::move(f)};
            }
        };
    }    // namespace detail

    template <typename S, typename F>
    auto let_value(S&& s, F&& f)
    {
        return detail::let_value_sender<S, F>{
            std::forward<S>(s), std::forward<F>(f)};
    }
}}}    // namespace hpx::execution::experimental
