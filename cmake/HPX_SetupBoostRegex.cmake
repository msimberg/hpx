# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

find_package(Boost ${Boost_MINIMUM_VERSION}
  QUIET MODULE
  COMPONENTS regex)

if(Boost_REGEX_FOUND)
  hpx_info("  regex")
else()
  hpx_error("Could not find Boost.Regex but HPX_WITH_TOOLS=On (the inspect \
  tool requires Boost.Regex). Either set it to off or provide a boost \
  installation including the regex library")
endif()

add_library(hpx::boost::regex INTERFACE IMPORTED)
set_property(TARGET hpx::boost::regex PROPERTY INTERFACE_LINK_LIBRARIES
  ${Boost_REGEX_LIBRARIES})
