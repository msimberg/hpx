# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Default location is $HPX_ROOT/libs/module_support/include
set(module_support_headers hpx/module_support.hpp)
set(module_support_sources module_support.cpp)

include(HPX_AddModule)
add_hpx_module(module_support
  FORCE_LINKING_GEN
  GLOBAL_HEADER_GEN OFF
  SOURCES ${module_support_sources}
  HEADERS ${module_support_headers}
)
