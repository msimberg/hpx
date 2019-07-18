# Copyright (c) 2018 Mikael Simberg
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(example_programs)

hpx_warn("about to add hpx::compute::kokkos examples")

if(HPX_WITH_KOKKOS)
  hpx_warn("HPX_WITH_KOKKOS=ON")
  set(example_programs ${example_programs}
      parallel_dispatch
      parallel_dispatch_executor
    )

  set(parallel_dispatch_PARAMETERS THREADS_PER_LOCALITY 4)

  include_directories(${Kokkos_INCLUDE_DIRS_RET})
  if(HPX_WITH_THRUST)
    hpx_warn("including \"${CMAKE_SOURCE_DIR}/thrust\"")
    include_directories("${CMAKE_SOURCE_DIR}/thrust")
  endif()
endif()

if(HPX_WITH_OPENMP_WORKER_THREADS)
  set(example_programs ${example_programs}
      openmp_partition
      openmp_worker_threads_and_executor
    )
endif()

foreach(example_program ${example_programs})
   hpx_warn("adding hpx::compute::kokkos examples")
   set(sources
       ${example_program}.cpp)

  source_group("Source Files" FILES ${sources})

  # add example executable
  if(HPX_WITH_KOKKOS)
    add_hpx_executable(${example_program}
      SOURCES ${sources}
      ${${example_program}_FLAGS}
      FOLDER "Examples/Compute/Kokkos"
      DEPENDENCIES PRIVATE kokkos
      COMPONENT_DEPENDENCIES iostreams)
  else()
    add_hpx_executable(${example_program}
      SOURCES ${sources}
      ${${example_program}_FLAGS}
      FOLDER "Examples/Compute/Kokkos"
      COMPONENT_DEPENDENCIES iostreams)
  endif()

  if(HPX_WITH_EXAMPLES_OPENMP)
    hpx_warn("OpenMP_CXX_FLAGS=${OpenMP_CXX_FLAGS}")
    set_target_properties(${example_program}
      PROPERTIES COMPILE_FLAGS "${OpenMP_CXX_FLAGS}")
    if(NOT MSVC)
      set_target_properties(${example_program}
        PROPERTIES LINK_FLAGS "${OpenMP_CXX_FLAGS}")
    endif()
  endif()

  if(HPX_WITH_THRUST)
      target_compile_definitions(${example_program}
          PUBLIC "THRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP")
  endif()

  # add a custom target for this example
  add_hpx_pseudo_target(examples.compute.kokkos.${example_program})

  # make pseudo-targets depend on master pseudo-target
  add_hpx_pseudo_dependencies(examples.compute.kokkos
                              examples.compute.kokkos.${example_program})

  # add dependencies to pseudo-target
  add_hpx_pseudo_dependencies(examples.compute.kokkos.${example_program}
                              ${example_program})

  if(HPX_WITH_TESTS AND HPX_WITH_TESTS_EXAMPLES)
    add_hpx_example_test("kokkos" ${example_program} ${${example_program}_PARAMETERS})
    add_hpx_pseudo_target(tests.examples.compute.kokkos.${example_program})
    add_hpx_pseudo_dependencies(tests.examples.compute.kokkos
                                tests.examples.compute.kokkos.${example_program})
    add_hpx_pseudo_dependencies(tests.examples.compute.kokkos.${example_program}
                                ${example_program})
  endif()
endforeach()
