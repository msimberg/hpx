# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(tests
    format
)

foreach(test ${tests})
  set(sources ${test}.cpp)

  source_group("Source Files" FILES ${sources})

  add_executable(${test}_test ${sources})
  target_link_libraries(${test}_test hpx_format hpx_testing)

  add_hpx_unit_test("format" ${test} ${${test}_PARAMETERS})

  # add a custom target for this example
  add_hpx_pseudo_target(tests.unit.format.${test})

  # make pseudo-targets depend on master pseudo-target
  add_hpx_pseudo_dependencies(tests.unit.format
                              tests.unit.format.${test})

  # add dependencies to pseudo-target
  add_hpx_pseudo_dependencies(tests.unit.format.${test}
                              ${test}_test)
endforeach()
