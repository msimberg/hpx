# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

project(HPX.errors CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HPX_AddDefinitions)
include(HPX_Option)

hpx_option(HPX_ERRORS_WITH_TESTS
  BOOL
  "Build HPX errors module tests. (default: ON)"
  ON ADVANCED
  CATEGORY "Modules")

hpx_option(HPX_ERRORS_WITH_DEPRECATION_WARNINGS
  BOOL
  "Enable warnings for deprecated facilities. (default: ${HPX_WITH_DEPRECATION_WARNINGS})"
  ${HPX_WITH_DEPRECATION_WARNINGS} ADVANCED
  CATEGORY "Modules")
if(HPX_ERRORS_WITH_DEPRECATION_WARNINGS)
  hpx_add_config_define_namespace(
    DEFINE HPX_ERRORS_HAVE_DEPRECATION_WARNINGS
    NAMESPACE ERRORS)
endif()

# Added in 1.4.0
hpx_option(HPX_ERRORS_WITH_COMPATIBILITY_HEADERS
  BOOL
  "Enable compatibility headers for old headers"
  ON ADVANCED
  CATEGORY "Modules")

message(STATUS "errors: Configuring")

set(headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/errors.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/errors/error.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/errors/error_code.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/errors/exception_fwd.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/errors/exception_info.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/errors/exception_list.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/errors/throw_exception.hpp
)

if(HPX_ERRORS_WITH_COMPATIBILITY_HEADERS)
  set(compat_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/error.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/error_code.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/exception_fwd.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/exception_info.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/exception_list.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/throw_exception.hpp
  )
endif()

set(sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/error_code.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/exception.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/exception_list.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/throw_exception.cpp
)

add_library(hpx_errors STATIC ${sources} ${headers} ${compat_headers})

target_link_libraries(hpx_errors hpx_assertion hpx_config hpx_format hpx_logging hpx_util)
target_include_directories(hpx_errors PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

if(HPX_ERRORS_WITH_COMPATIBILITY_HEADERS)
  target_include_directories(hpx_errors PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility>)
endif()

target_compile_definitions(hpx_errors PUBLIC
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
  ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src
  CLASS "Source Files"
  TARGETS ${sources})

if(HPX_ERRORS_WITH_COMPATIBILITY_HEADERS)
  add_hpx_source_group(
    NAME hpx
    ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    CLASS "Header Files"
    TARGETS ${headers})
endif()

install(TARGETS hpx_errors EXPORT HPXTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT config
)
hpx_export_targets(hpx_errors)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT config)

if(HPX_ERRORS_WITH_COMPATIBILITY_HEADERS)
  install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT errors)
endif()

write_config_defines_file(
  NAMESPACE ERRORS
  FILENAME "${CMAKE_BINARY_DIR}/hpx/errors/config/defines.hpp")

write_config_defines_file(
  NAMESPACE ERRORS
  FILENAME "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpx/errors/config/defines.hpp")

add_subdirectory(examples)
add_subdirectory(tests)

message(STATUS "errors: Configuring done")
