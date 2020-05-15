//  Copyright (c) 2007-2017 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/errors.hpp>

#include <string>

namespace hpx { namespace util {
    HPX_CORE_EXPORT std::string regex_from_pattern(
        std::string const& pattern, error_code& ec = throws);
}}    // namespace hpx::util
