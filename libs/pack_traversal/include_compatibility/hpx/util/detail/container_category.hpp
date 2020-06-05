//  Copyright (c) 2019 Ste||ar Group
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/pack_traversal/config/defines.hpp>
#include <hpx/pack_traversal/detail/container_category.hpp>

#if HPX_PACK_TRAVERSAL_HAVE_DEPRECATION_WARNINGS
#if defined(HPX_MSVC)
#pragma message(                                                               \
    "The header hpx/util/detail/container_category.hpp is deprecated, \
    please include hpx/pack_traversal/detail/container_category.hpp instead")
#else
#warning "The header hpx/util/detail/container_category.hpp is deprecated, \
    please include hpx/pack_traversal/detail/container_category.hpp instead"
#endif
#endif
