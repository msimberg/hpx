# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

if (NOT HPX_WITH_TESTS)
  return()
endif()
if (NOT HPX_TOPOLOGY_WITH_TESTS)
  message(STATUS "Tests for topology disabled")
  return()
endif()

if (HPX_WITH_TESTS_UNIT)
  add_hpx_pseudo_target(tests.unit.topology)
  add_hpx_pseudo_dependencies(tests.unit tests.unit.topology)
  add_subdirectory(unit)
endif()

if (HPX_WITH_TESTS_REGRESSIONS)
  add_hpx_pseudo_target(tests.regressions.topology)
  add_hpx_pseudo_dependencies(tests.regressions tests.regressions.topology)
  add_subdirectory(regressions)
endif()

if (HPX_WITH_TESTS_BENCHMARKS)
  add_hpx_pseudo_target(tests.performance.topology)
  add_hpx_pseudo_dependencies(tests.performance tests.performance.topology)
  add_subdirectory(performance)
endif()

if (HPX_WITH_TESTS_HEADERS)
  add_hpx_lib_header_tests(topology)
endif()
