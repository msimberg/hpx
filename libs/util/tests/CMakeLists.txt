# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

if (NOT HPX_WITH_TESTS)
  return()
endif()
if (NOT HPX_UTIL_WITH_TESTS)
  message(STATUS "Tests for util disabled")
  return()
endif()

# NOTE: Some util functionality is still in the main HPX library. We don't
# duplicate the pseudo target if testing is enabled for the main library.
if (HPX_WITH_TESTS_UNIT AND NOT TARGET tests.unit.util)
  add_hpx_pseudo_target(tests.unit.util)
  add_hpx_pseudo_dependencies(tests.unit tests.unit.util)
  add_subdirectory(unit)
endif()

if (HPX_WITH_TESTS_REGRESSIONS AND NOT TARGET tests.regressions.util)
  add_hpx_pseudo_target(tests.regressions.util)
  add_hpx_pseudo_dependencies(tests.regressions tests.regressions.util)
  add_subdirectory(regressions)
endif()

if (HPX_WITH_TESTS_BENCHMARKS AND NOT TARGET tests.performance.util)
  add_hpx_pseudo_target(tests.performance.util)
  add_hpx_pseudo_dependencies(tests.performance tests.performance.util)
  add_subdirectory(performance)
endif()

if (HPX_WITH_TESTS_HEADERS)
  add_hpx_lib_header_tests(util)
endif()
