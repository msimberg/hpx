# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

project(HPX.timing CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HPX_AddDefinitions)
include(HPX_Option)

hpx_option(HPX_TIMING_WITH_TESTS
  BOOL
  "Build HPX timing module tests. (default: ON)"
  ON ADVANCED
  CATEGORY "Modules")

hpx_option(HPX_TIMING_WITH_DEPRECATION_WARNINGS
  BOOL
  "Enable warnings for deprecated facilities. (default: ${HPX_WITH_DEPRECATION_WARNINGS})"
  ${HPX_WITH_DEPRECATION_WARNINGS} ADVANCED
  CATEGORY "Modules")
if(HPX_TIMING_WITH_DEPRECATION_WARNINGS)
  hpx_add_config_define_namespace(
    DEFINE HPX_TIMING_HAVE_DEPRECATION_WARNINGS
    NAMESPACE TIMING)
endif()

# Added in 1.4.0
hpx_option(HPX_TIMING_WITH_COMPATIBILITY_HEADERS
  BOOL
  "Enable compatibility headers for old headers"
  ON ADVANCED
  CATEGORY "Modules")

if(HPX_TIMING_WITH_COMPATIBILITY_HEADERS)
  hpx_add_config_define_namespace(
    DEFINE HPX_TIMING_HAVE_COMPATIBILITY_HEADERS
    NAMESPACE TIMING)
endif()

message(STATUS "timing: Configuring")

set(headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/timing.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/timing/high_resolution_clock.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/timing/high_resolution_timer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/timing/scoped_timer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/timing/steady_clock.hpp
)

if(HPX_TIMING_WITH_COMPATIBILITY_HEADERS)
  set(compat_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/high_resolution_clock.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/high_resolution_timer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/scoped_timer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/steady_clock.hpp
  )
endif()

# cmake will not create a separate VS project without any source files, thus
# this adds a dummy (empty) source file to the target
set(sources ${CMAKE_CURRENT_SOURCE_DIR}/timing.cpp)

add_library(hpx_timing STATIC ${headers} ${compat_headers})

target_link_libraries(hpx_timing hpx_config)
target_include_directories(hpx_timing INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(HPX_TIMING_WITH_COMPATIBILITY_HEADERS)
  target_include_directories(hpx_timing PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility>)
endif()

include(HPX_AddSourceGroup)
add_hpx_source_group(
  NAME hpx
  ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  CLASS "Header Files"
  TARGETS ${headers})
add_hpx_source_group(
  NAME hpx_timing
  ROOT ${CMAKE_CURRENT_SOURCE_DIR}
  CLASS "Source Files"
  TARGETS ${sources})

if(HPX_TIMING_WITH_COMPATIBILITY_HEADERS)
  add_hpx_source_group(
    NAME hpx
    ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    CLASS "Header Files"
    TARGETS ${compat_headers})
endif()

install(TARGETS hpx_timing EXPORT HPXTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT timing
)
hpx_export_targets(hpx_timing)

install(
  DIRECTORY include/hpx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT timing)

if(HPX_TIMING_WITH_COMPATIBILITY_HEADERS)
  install(
    DIRECTORY include_compatibility/hpx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT timing)
endif()

write_config_defines_file(
  NAMESPACE TIMING
  FILENAME "${CMAKE_BINARY_DIR}/hpx/timing/config/defines.hpp")

write_config_defines_file(
  NAMESPACE TIMING
  FILENAME "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpx/timing/config/defines.hpp")

add_subdirectory(examples)
add_subdirectory(tests)

message(STATUS "timing: Configuring done")
