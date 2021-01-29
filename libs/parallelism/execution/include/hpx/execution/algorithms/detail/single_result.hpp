//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/type_support/pack.hpp>

namespace hpx {
    namespace execution {
        namespace experimental {
            namespace detail {
    template <typename... Variants>
    struct sync_wait_single_result
    {
        static_assert(sizeof...(Variants) == 1,
            "sync_wait expects the predecessor sender to have a single "
            "variant "
            "in sender_traits<>::value_types");
    };

    template <typename T>
    struct sync_wait_single_result<hpx::util::pack<hpx::util::pack<T>>>
    {
        using type = T;
    };

    template <>
    struct sync_wait_single_result<hpx::util::pack<hpx::util::pack<>>>
    {
        using type = void;
    };
}}}}    // namespace hpx::execution::experimental::detail
