# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(threadmanager_headers hpx/threadmanager.hpp)

set(threadmanager_compat_headers hpx/runtime/threads/threadmanager.hpp)

set(threadmanager_sources threadmanager.cpp)

include(HPX_AddModule)
add_hpx_module(
  full threadmanager
  COMPATIBILITY_HEADERS ON # Added in 1.4.0
  DEPRECATION_WARNINGS
  GLOBAL_HEADER_GEN OFF
  SOURCES ${threadmanager_sources}
  HEADERS ${threadmanager_headers}
  COMPAT_HEADERS ${threadmanager_compat_headers}
  MODULE_DEPENDENCIES
    hpx_command_line_handling hpx_execution hpx_program_options
    hpx_resource_partitioner hpx_runtime_configuration hpx_core
  CMAKE_SUBDIRS examples tests
)
