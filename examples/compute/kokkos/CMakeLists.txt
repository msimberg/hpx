# Copyright (c) 2018 Mikael Simberg
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(example_programs)

if(HPX_WITH_KOKKOS)
  set(example_programs ${example_programs}
      parallel_dispatch
      parallel_dispatch_executor
    )

  set(parallel_dispatch_PARAMETERS THREADS_PER_LOCALITY 4)

  include_directories(${Kokkos_INCLUDE_DIRS_RET})
endif()

foreach(example_program ${example_programs})
   set(sources
       ${example_program}.cpp)

  source_group("Source Files" FILES ${sources})

  # add example executable
  add_hpx_executable(${example_program}
                     SOURCES ${sources}
                     ${${example_program}_FLAGS}
                     FOLDER "Examples/Compute/Kokkos"
                     DEPENDENCIES PRIVATE kokkos
                     COMPONENT_DEPENDENCIES iostreams)

  # add a custom target for this example
  add_hpx_pseudo_target(examples.compute.kokkos.${example_program})

  # make pseudo-targets depend on master pseudo-target
  add_hpx_pseudo_dependencies(examples.compute.kokkos
                              examples.compute.kokkos.${example_program})

  # add dependencies to pseudo-target
  add_hpx_pseudo_dependencies(examples.compute.kokkos.${example_program}
                              ${example_program}_exe)

  if(HPX_WITH_TESTS AND HPX_WITH_TESTS_EXAMPLES)
    add_hpx_example_test("kokkos" ${example_program} ${${example_program}_PARAMETERS})
    add_hpx_pseudo_target(tests.examples.compute.kokkos.${example_program})
    add_hpx_pseudo_dependencies(tests.examples.compute.kokkos
                                tests.examples.compute.kokkos.${example_program})
    add_hpx_pseudo_dependencies(tests.examples.compute.kokkos.${example_program}
                                ${example_program}_exe)
  endif()
endforeach()
