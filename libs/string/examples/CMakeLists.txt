# Copyright (c) 2020 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

if (HPX_WITH_EXAMPLES)
  add_hpx_pseudo_target(examples.modules.string)
  add_hpx_pseudo_dependencies(examples.modules examples.modules.string)
  if (HPX_WITH_TESTS AND HPX_WITH_TESTS_EXAMPLES AND HPX_STRING_WITH_TESTS)
    add_hpx_pseudo_target(tests.examples.modules.string)
    add_hpx_pseudo_dependencies(tests.examples.modules tests.examples.modules.string)
  endif()
else()
  return()
endif()

set(example_programs
  conv_example
  find_example
  predicate_example
  replace_example
  rle_example
  split_example
  trim_example
)

foreach(example_program ${example_programs})
  set(sources ${example_program}.cpp)

  source_group("Source Files" FILES ${sources})

  # add example executable
  add_hpx_executable(${example_program}
                     INTERNAL_FLAGS
                     SOURCES ${sources}
                     ${${example_program}_FLAGS}
                     NOLIBS
                     DEPENDENCIES hpx_string
                     FOLDER "Examples/String")

  add_hpx_example_target_dependencies("modules.string" ${example_program})

  if (HPX_WITH_TESTS AND HPX_WITH_TESTS_EXAMPLES AND HPX_STRING_WITH_TESTS)
    add_hpx_example_test("modules.string" ${example_program}
      ${${example_program}_PARAMETERS})
  endif()
endforeach()
