//  Copyright (c) 2007-2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)

namespace hpx { namespace actions {

    class HPX_EXPORT continuation;

    template <typename Result, typename RemoteResult = Result>
    struct typed_continuation;

    template <typename Result, typename RemoteResult, typename F,
        typename... Ts>
    void trigger(
        typed_continuation<Result, RemoteResult>&& cont, F&& f, Ts&&... vs);
}}    // namespace hpx::actions
#endif
