# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

project(HPX.format CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HPX_AddDefinitions)
include(HPX_Option)

hpx_option(HPX_FORMAT_WITH_TESTS
  BOOL
  "Build HPX format module tests. (default: ON)"
  ON ADVANCED
  CATEGORY "Modules")

hpx_option(HPX_FORMAT_WITH_DEPRECATION_WARNINGS
  BOOL
  "Enable warnings for deprecated facilities. (default: ${HPX_WITH_DEPRECATION_WARNINGS})"
  ${HPX_WITH_DEPRECATION_WARNINGS} ADVANCED
  CATEGORY "Modules")
if(HPX_FORMAT_WITH_DEPRECATION_WARNINGS)
  hpx_add_config_define_namespace(
    DEFINE HPX_FORMAT_HAVE_DEPRECATION_WARNINGS
    NAMESPACE FORMAT)
endif()

# Added in 1.4.0
hpx_option(HPX_FORMAT_WITH_COMPATIBILITY_HEADERS
  BOOL
  "Enable compatibility headers for old headers"
  ON ADVANCED
  CATEGORY "Modules")

if(HPX_FORMAT_WITH_COMPATIBILITY_HEADERS)
  hpx_add_config_define_namespace(
    DEFINE HPX_FORMAT_HAVE_COMPATIBILITY_HEADERS
    NAMESPACE FORMAT)
endif()

message(STATUS "format: Configuring")

set(headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/format.hpp
)

if(HPX_FORMAT_WITH_COMPATIBILITY_HEADERS)
  set(compat_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/format.hpp
  )
endif()

set(sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/format.cpp
)

add_library(hpx_format STATIC ${sources} ${headers} ${compat_headers})

target_link_libraries(hpx_format hpx_assertion hpx_config)
target_include_directories(hpx_format PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

if(HPX_FORMAT_WITH_COMPATIBILITY_HEADERS)
  target_include_directories(hpx_format PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility>)
endif()

target_compile_definitions(hpx_format PUBLIC
  $<$<CONFIG:Debug>:DEBUG>
  $<$<CONFIG:Debug>:_DEBUG>
)

include(HPX_AddSourceGroup)
add_hpx_source_group(
  NAME hpx
  ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  CLASS "Header Files"
  TARGETS ${headers})
add_hpx_source_group(
  NAME hpx
  ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  CLASS "Source Files"
  TARGETS ${sources})

if(HPX_FORMAT_WITH_COMPATIBILITY_HEADERS)
  add_hpx_source_group(
    NAME hpx
    ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    CLASS "Header Files"
    TARGETS ${compat_headers})
endif()

install(TARGETS hpx_format EXPORT HPXTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT config
)
hpx_export_targets(hpx_format)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT config)

if(HPX_FORMAT_WITH_COMPATIBILITY_HEADERS)
  install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT format)
endif()

write_config_defines_file(
  NAMESPACE FORMAT
  FILENAME "${CMAKE_BINARY_DIR}/hpx/format/config/defines.hpp")

write_config_defines_file(
  NAMESPACE FORMAT
  FILENAME "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpx/format/config/defines.hpp")

add_subdirectory(examples)
add_subdirectory(tests)

message(STATUS "format: Configuring done")
