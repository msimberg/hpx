# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT HPX_WITH_DISTRIBUTED_RUNTIME)
  return()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Default location is $HPX_ROOT/modules/collectives/include
set(collectives_headers
    hpx/collectives/all_reduce.hpp
    hpx/collectives/all_to_all.hpp
    hpx/collectives/barrier.hpp
    hpx/collectives/broadcast.hpp
    hpx/collectives/broadcast_direct.hpp
    hpx/collectives/detail/communicator.hpp
    hpx/collectives/fold.hpp
    hpx/collectives/gather.hpp
    hpx/collectives/latch.hpp
    hpx/collectives/reduce.hpp
    hpx/collectives/spmd_block.hpp
    hpx/collectives/detail/barrier_node.hpp
    hpx/collectives/detail/latch.hpp
)

# Default location is $HPX_ROOT/modules/collectives/include_compatibility
set(collectives_compat_headers
    hpx/lcos/barrier.hpp
    hpx/lcos/broadcast.hpp
    hpx/lcos/fold.hpp
    hpx/lcos/gather.hpp
    hpx/lcos/latch.hpp
    hpx/lcos/reduce.hpp
    hpx/lcos/spmd_block.hpp
)

# Default location is $HPX_ROOT/modules/collectives/src
set(collectives_sources barrier.cpp latch.cpp detail/barrier_node.cpp)

include(HPX_AddModule)
add_hpx_module(
  full collectives
  COMPATIBILITY_HEADERS ON # Added in 1.4.0
  DEPRECATION_WARNINGS FORCE_LINKING_GEN
  SOURCES ${collectives_sources}
  HEADERS ${collectives_headers}
  COMPAT_HEADERS ${collectives_compat_headers}
  MODULE_DEPENDENCIES
    hpx_async
    hpx_execution
    hpx_executors
    hpx_local_lcos
    hpx_pack_traversal
    hpx_performance_counters
    hpx_program_options
    hpx_runtime_configuration
    hpx_threading
    hpx_threadmanager
    hpx_core
  CMAKE_SUBDIRS examples tests
)
