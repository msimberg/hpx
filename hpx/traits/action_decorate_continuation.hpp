//  Copyright (c) 2007-2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/actions_base/continuation_fwd.hpp>
#include <hpx/actions_base/traits/action_continuation.hpp>

namespace hpx { namespace traits
{
    ///////////////////////////////////////////////////////////////////////////
    // Customization point for action capabilities
    template <typename Action, typename Enable = void>
    struct action_decorate_continuation
    {
        using continuation_type =
            typename traits::action_continuation<Action>::type;

        static bool call(continuation_type& /*cont*/)
        {
            // by default we do nothing
            return false; // continuation has not been modified
        }
    };
}}
#endif
