//  Copyright (c) 2019 Auriane Reverdell
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/datastructures/config/defines.hpp>
#include <hpx/datastructures/traits/supports_streaming_with_any.hpp>

#if HPX_DATASTRUCTURES_HAVE_DEPRECATION_WARNINGS
#if defined(HPX_MSVC)
#pragma message(                                                               \
    "The header hpx/traits/supports_streaming_with_any.hpp is deprecated, \
    please include hpx/datastructures/traits/supports_streaming_with_any.hpp \
    instead.")
#else
#warning                                                                       \
    "The header hpx/traits/supports_streaming_with_any.hpp is deprecated, \
    please include hpx/datastructures/traits/supports_streaming_with_any.hpp \
    instead."
#endif
#endif
