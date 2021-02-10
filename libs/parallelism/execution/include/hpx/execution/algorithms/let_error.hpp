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

#include <exception>
#include <type_traits>
#include <utility>

namespace hpx { namespace execution { namespace experimental {
    namespace detail {
        // TODO: Is the lifetime of the error from s handled correctly?
        // Currently nothing is done to extend its lifetime.
        template <typename R, typename F>
        struct let_error_receiver
        {
            std::decay_t<R> r;
            std::decay_t<F> f;

            template <typename R_, typename F_>
            let_error_receiver(R_&& r, F_&& f)
              : r(std::forward<R_>(r))
              , f(std::forward<F_>(f))
            {
            }

            template <typename E>
            void set_error(E&& e) noexcept
            {
                // NOTE: e is not forwarded. It should be possible to take
                // it by reference.
                hpx::execution::experimental::start(
                    hpx::execution::experimental::connect(
                        HPX_INVOKE(f, e), std::move(r)));
            }

            void set_done() noexcept
            {
                hpx::execution::experimental::set_done(std::move(r));
            };

            template <typename... Ts>
            void set_value(Ts&&... ts) noexcept
            {
                hpx::execution::experimental::set_value(
                    std::move(r), std::forward<Ts>(ts)...);
            }
        };

        template <typename S, typename R, typename F>
        struct let_error_operation_state
        {
            using operation_state_type =
                std::decay_t<typename hpx::util::invoke_result<
                    hpx::execution::experimental::connect_t, S&&,
                    let_error_receiver<R, F>>::type>;
            operation_state_type os;

            template <typename S_, typename R_, typename F_>
            let_error_operation_state(S_&& s, R_&& r, F_&& f)
              : os{hpx::execution::experimental::connect(std::move(s),
                    let_error_receiver<R, F>(
                        std::forward<R_>(r), std::forward<F_>(f)))}
            {
            }

            void start() noexcept
            {
                hpx::execution::experimental::start(os);
            }
        };

        template <typename S, typename F>
        struct let_error_sender
        {
            std::decay_t<S> s;
            std::decay_t<F> f;

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
                return let_error_operation_state<S, R, F>{
                    std::move(s), std::move(r), std::move(f)};
            }
        };
    }    // namespace detail

    template <typename S, typename F>
    auto let_error(S&& s, F&& f)
    {
        return detail::let_error_sender<S, F>{
            std::forward<S>(s), std::forward<F>(f)};
    }
}}}    // namespace hpx::execution::experimental
