# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(resource_partitioner_headers
    hpx/resource_partitioner/detail/create_partitioner.hpp
    hpx/resource_partitioner/detail/partitioner.hpp
    hpx/resource_partitioner/partitioner.hpp
    hpx/resource_partitioner/partitioner_fwd.hpp
)

set(resource_partitioner_compat_headers
    hpx/runtime/resource/detail/create_partitioner.hpp
    hpx/runtime/resource/detail/partitioner.hpp
    hpx/runtime/resource/partitioner.hpp
    hpx/runtime/resource/partitioner_fwd.hpp
)

set(resource_partitioner_sources detail_partitioner.cpp partitioner.cpp)

include(HPX_AddModule)
add_hpx_module(
  full resource_partitioner
  COMPATIBILITY_HEADERS ON # Added in 1.4.0
  DEPRECATION_WARNINGS
  GLOBAL_HEADER_GEN ON
  SOURCES ${resource_partitioner_sources}
  HEADERS ${resource_partitioner_headers}
  COMPAT_HEADERS ${resource_partitioner_compat_headers}
  MODULE_DEPENDENCIES
    hpx_command_line_handling
    hpx_execution
    hpx_mpi_base
    hpx_pack_traversal
    hpx_program_options
    hpx_runtime_configuration
    hpx_core
  CMAKE_SUBDIRS examples tests
)
