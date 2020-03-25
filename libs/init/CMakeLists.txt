# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(init_headers
  hpx/hpx_main.hpp
  hpx/hpx_main_impl.hpp)

set(init_sources
  hpx_main.cpp
  hpx_main_argc_argv.cpp
  hpx_main_variables_map.cpp
  hpx_main_winsocket.cpp
  hpx_user_main.cpp
  hpx_user_main_argc_argv.cpp
  main.cpp)

if(HPX_WITH_DYNAMIC_HPX_MAIN AND (("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") OR (APPLE)))
  list(APPEND init_sources hpx_wrap.cpp)
endif()

include(HPX_AddModule)
add_hpx_module(init
  GLOBAL_HEADER_GEN OFF
  SOURCES ${init_sources}
  HEADERS ${init_headers}
  DEPENDENCIES hpx_config
)
