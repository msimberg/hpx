//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/execution_base/sender.hpp>
#include <hpx/functional/invoke_result.hpp>
#include <hpx/functional/tag_fallback_invoke.hpp>
#include <hpx/type_support/pack.hpp>

#include <atomic>
#include <cstddef>
#include <exception>
#include <type_traits>
#include <utility>

namespace hpx { namespace execution { namespace experimental {
    namespace detail {
        template <typename R, typename T>
        struct just_on_receiver
        {
            typename std::decay<R>::type r;
            typename std::decay<T>::type t;

            template <typename R_, typename T_>
            just_on_receiver(R_&& r, T_&& t)
              : r(std::forward<R>(r))
              , t(std::forward<T>(t))
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
                hpx::execution::experimental::set_value(
                    std::move(r), std::move(t));
            }
        };

        template <typename Scheduler, typename T>
        struct just_on_sender
        {
            typename std::decay<Scheduler>::type scheduler;
            typename std::decay<T>::type t;

            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types = Variant<Tuple<T>>;

            using scheduler_sender_type =
                std::decay_t<typename hpx::util::invoke_result<
                    hpx::execution::experimental::schedule_t, Scheduler>::type>;

            template <template <typename...> class Variant>
            using error_types =
                typename hpx::execution::experimental::sender_traits<
                    scheduler_sender_type>::template error_types<Variant>;

            static constexpr bool sends_done = false;

            template <typename R>
            auto connect(R&& r)
            {
                auto s = hpx::execution::experimental::schedule(scheduler);
                return hpx::execution::experimental::connect(std::move(s),
                    just_on_receiver<R, T>(std::forward<R>(r), std::move(t)));
            }
        };
    }    // namespace detail

    HPX_INLINE_CONSTEXPR_VARIABLE struct just_on_t final
      : hpx::functional::tag_fallback<just_on_t>
    {
    private:
        template <typename Scheduler, typename T>
        friend constexpr HPX_FORCEINLINE auto tag_fallback_invoke(
            just_on_t, Scheduler&& scheduler, T&& t)
        {
            return detail::just_on_sender<Scheduler, T>{
                std::forward<Scheduler>(scheduler), std::forward<T>(t)};
        }
    } just_on{};
}}}    // namespace hpx::execution::experimental
