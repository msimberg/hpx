# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(command_line_handling_headers
    hpx/command_line_handling/command_line_handling.hpp
    hpx/command_line_handling/parse_command_line.hpp
)

set(command_line_handling_compat_headers hpx/util/command_line_handling.hpp
                                         hpx/util/parse_command_line.hpp
)

set(command_line_handling_sources command_line_handling.cpp
                                  parse_command_line.cpp
)

include(HPX_AddModule)
add_hpx_module(
  full command_line_handling
  COMPATIBILITY_HEADERS ON
  DEPRECATION_WARNINGS
  GLOBAL_HEADER_GEN ON
  SOURCES ${command_line_handling_sources}
  HEADERS ${command_line_handling_headers}
  COMPAT_HEADERS ${command_line_handling_compat_headers}
  MODULE_DEPENDENCIES
    hpx_batch_environments
    hpx_execution
    hpx_mpi_base
    hpx_program_options
    hpx_runtime_configuration
    hpx_threading
    hpx_core
  CMAKE_SUBDIRS
)
