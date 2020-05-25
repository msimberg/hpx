//  Copyright (c) 2019-2020 The STE||AR GROUP
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/synchronization/stop_token.hpp>

namespace hpx { namespace synchronization {

    struct force_linking_helper
    {
        void (*intrusive_ptr_add_ref)(hpx::detail::stop_state* p);
    };

    force_linking_helper& force_linking();
}}    // namespace hpx::synchronization
