# Copyright (c) 2020 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(HPX_Message)
include(HPX_AddPseudoDependencies)
include(HPX_AddPseudoTarget)

# add example pseudo targets needed for modules
if(HPX_WITH_EXAMPLES)
  add_hpx_pseudo_target(examples.modules)
  add_hpx_pseudo_dependencies(examples examples.modules)
endif()

# add test pseudo targets needed for modules
if(HPX_WITH_TESTS)
  if(HPX_WITH_TESTS_UNIT)
    add_hpx_pseudo_target(tests.unit.modules)
    add_hpx_pseudo_dependencies(tests.unit tests.unit.modules)
  endif()

  if(HPX_WITH_EXAMPLES AND HPX_WITH_TESTS_EXAMPLES)
    add_hpx_pseudo_target(tests.examples.modules)
    add_hpx_pseudo_dependencies(tests.examples tests.examples.modules)
  endif()

  if(HPX_WITH_TESTS_REGRESSIONS)
    add_hpx_pseudo_target(tests.regressions.modules)
    add_hpx_pseudo_dependencies(tests.regressions tests.regressions.modules)
  endif()

  if(HPX_WITH_TESTS_BENCHMARKS)
    add_hpx_pseudo_target(tests.performance.modules)
    add_hpx_pseudo_dependencies(tests.performance tests.performance.modules)
  endif()

  if(HPX_WITH_TESTS_HEADERS)
    add_hpx_pseudo_target(tests.headers.modules)
    add_hpx_pseudo_dependencies(tests.headers tests.headers.modules)
  endif()
endif()

hpx_info("")
hpx_info("Configuring modules:")

# Generate a file that lists all enabled modules for checks that might be
# optional based on the presence of a module or not
set(MODULE_ENABLED_MODULE_DEFINES)

# variables needed for config_strings_modules.hpp
set(CONFIG_STRINGS_MODULES_INCLUDES)
set(CONFIG_STRINGS_MODULES_ENTRIES)

set(_hpx_libs core full)

foreach(lib ${_hpx_libs})
  # NOTE: hpx_full (i.e. the target representing libhpx.so) is still created in
  # src/CMakeLists.txt. When all of hpx_full has been modularized the target
  # creation can move here as well.
  if(NOT ${lib} STREQUAL "full")
    add_library(hpx_${lib} SHARED src/dummy.cpp)
    add_library(hpx_${lib}_static STATIC src/dummy.cpp)

    install(
      TARGETS hpx_${lib}
      EXPORT HPXInternalTargets
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${lib}
    )
    install(
      TARGETS hpx_${lib}_static
      EXPORT HPXInternalTargets
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${lib}
    )

    hpx_export_internal_targets(hpx_${lib} hpx_${lib}_static)
  endif()

  add_subdirectory(${lib})
endforeach()

# add_hpx_module populates HPX_ENABLED_MODULES
foreach(module ${HPX_ENABLED_MODULES})
  string(TOUPPER ${module} uppercase_module)

  set(MODULE_ENABLED_MODULE_DEFINES
      "${MODULE_ENABLED_MODULE_DEFINES}#define HPX_HAVE_MODULE_${uppercase_module}\n"
  )
endforeach()

configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/templates/modules_enabled.hpp.in"
  "${PROJECT_BINARY_DIR}/modules/core/config/include/hpx/config/modules_enabled.hpp"
  @ONLY
)
