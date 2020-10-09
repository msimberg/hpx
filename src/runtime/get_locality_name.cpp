//  Copyright (c) 2007-2013 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/modules/async_distributed.hpp>
#include <hpx/async_distributed/apply.hpp>
#include <hpx/modules/errors.hpp>
#include <hpx/runtime/get_locality_name.hpp>
#include <hpx/runtime_local/runtime_local.hpp>

#include <hpx/actions_base/plain_action.hpp>
#include <hpx/runtime_distributed.hpp>

#include <string>

namespace hpx { namespace detail
{
    std::string get_locality_base_name()
    {
#if defined(HPX_HAVE_NETWORKING) && defined(HPX_HAVE_DISTRIBUTED_RUNTIME)
        runtime_distributed* rt = get_runtime_distributed_ptr();
        if (rt == nullptr)
        {
            HPX_THROW_EXCEPTION(invalid_status,
                "hpx::detail::get_locality_name",
                "the runtime system is not operational at this point");
            return "";
        }
        return rt->get_parcel_handler().get_locality_name();
#else
        return "console";
#endif
    }

    std::string get_locality_name()
    {
        std::string basename = get_locality_base_name();
        return basename + '#' + std::to_string(get_locality_id());
    }
}}

HPX_PLAIN_ACTION_ID(hpx::detail::get_locality_name, hpx_get_locality_name_action,
        hpx::actions::hpx_get_locality_name_action_id)

namespace hpx
{
    ///////////////////////////////////////////////////////////////////////////
    std::string get_locality_name()
    {
        return detail::get_locality_name();
    }

    future<std::string> get_locality_name(naming::id_type const& id)
    {
        return async<hpx_get_locality_name_action>(id);
    }
}
#endif
