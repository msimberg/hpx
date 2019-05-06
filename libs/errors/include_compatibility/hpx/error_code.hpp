//  Copyright (c) 2019 Mikael Simberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config.hpp>
#include <hpx/errors/config/defines.hpp>
#include <hpx/errors/error_code.hpp>

#if defined(HPX_ERRORS_HAVE_DEPRECATION_WARNINGS)
#if defined(HPX_MSVC)
#pragma message( \
    "The header hpx/error_code.hpp is deprecated, \
    please include hpx/errors/error_code.hpp instead")
#else
#warning \
    "The header hpx/error_code.hpp is deprecated, \
    please include hpx/errors/error_code.hpp instead"
#endif
#endif