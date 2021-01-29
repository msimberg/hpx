//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/execution_base/sender.hpp>

namespace hpx { namespace execution { namespace experimental {
    namespace detail {
        template <typename T>
        struct just_sender
        {
            typename std::decay<T>::type t;

            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = Variant<Tuple<T>>;

            template <template <typename...> class Variant>
            using error_types = Variant<>;

            static constexpr bool sends_done = false;

            template <typename R, typename T>
            struct operation_state
            {
                typename std::decay<R>::type r;
                typename std::decay<T>::type t;

                void start() noexcept
                {
                    hpx::execution::experimental::set_value(
                        std::move(r), std::move(t));
                }
            };

            template <typename R>
            auto connect(R&& r)
            {
                return operation_state<R, T>{std::forward<R>(r), std::move(t)};
            }
        };
    }    // namespace detail

    // TODO: Allow multiple values? No values?
    // TODO: Make CPO.
    template <typename T>
    auto just(T&& t)
    {
        return detail::just_sender<T>{std::forward<T>(t)};
    }
}}}    // namespace hpx::execution::experimental
