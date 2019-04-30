# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

project(HPX.testing CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HPX_AddDefinitions)
include(HPX_Option)

hpx_option(HPX_TESTING_WITH_TESTS
  BOOL
  "Build HPX testing module tests. (default: ON)"
  ON ADVANCED
  CATEGORY "Modules")

hpx_option(HPX_TESTING_WITH_DEPRECATION_WARNINGS
  BOOL
  "Enable warnings for deprecated facilities. (default: ${HPX_WITH_DEPRECATION_WARNINGS})"
  ${HPX_WITH_DEPRECATION_WARNINGS} ADVANCED
  CATEGORY "Modules")
if(HPX_TESTING_WITH_DEPRECATION_WARNINGS)
  hpx_add_config_define_namespace(
    DEFINE HPX_TESTING_HAVE_DEPRECATION_WARNINGS
    NAMESPACE TESTING)
endif()

# Added in 1.4.0
hpx_option(HPX_TESTING_WITH_COMPATIBILITY_HEADERS
  BOOL
  "Enable compatibility headers for old headers"
  ON ADVANCED
  CATEGORY "Modules")

message(STATUS "testing: Configuring")

set(headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/testing.hpp
)

if(HPX_TESTING_WITH_COMPATIBILITY_HEADERS)
  set(compat_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/lightweight_test.hpp
  )
endif()

set(sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/testing.cpp
)

add_library(hpx_testing STATIC ${sources} ${headers} ${compat_headers})

target_link_libraries(hpx_testing hpx_assertion hpx_config)
target_include_directories(hpx_testing PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

if(HPX_TESTING_WITH_COMPATIBILITY_HEADERS)
  target_include_directories(hpx_testing PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility>)
endif()

include(HPX_AddSourceGroup)
add_hpx_source_group(
  NAME hpx
  ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  CLASS "Header Files"
  TARGETS ${headers})
add_hpx_source_group(
  NAME hpx
  ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src
  CLASS "Source Files"
  TARGETS ${sources})

if(HPX_TESTING_WITH_COMPATIBILITY_HEADERS)
  add_hpx_source_group(
    NAME hpx
    ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
    CLASS "Header Files"
    TARGETS ${compat_headers})
endif()

set_target_properties(hpx_testing PROPERTIES
  FOLDER "Core/Modules")

install(TARGETS hpx_testing EXPORT HPXTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT testing
)
hpx_export_targets(hpx_testing)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT testing)

if(HPX_TESTING_WITH_COMPATIBILITY_HEADERS)
  install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT testing)
endif()

write_config_defines_file(
  NAMESPACE TESTING
  FILENAME "${CMAKE_BINARY_DIR}/hpx/testing/config/defines.hpp")

write_config_defines_file(
  NAMESPACE TESTING
  FILENAME "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpx/testing/config/defines.hpp")

add_subdirectory(examples)
add_subdirectory(tests)

message(STATUS "testing: Configuring done")
