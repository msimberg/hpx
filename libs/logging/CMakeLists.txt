# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

project(HPX.logging CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HPX_AddDefinitions)
include(HPX_Option)

hpx_option(HPX_LOGGING_WITH_TESTS
  BOOL
  "Build HPX logging module tests. (default: ON)"
  ON ADVANCED
  CATEGORY "Modules")

hpx_option(HPX_LOGGING_WITH_DEPRECATION_WARNINGS
  BOOL
  "Enable warnings for deprecated facilities. (default: ${HPX_WITH_DEPRECATION_WARNINGS})"
  ${HPX_WITH_DEPRECATION_WARNINGS} ADVANCED
  CATEGORY "Modules")
if(HPX_LOGGING_WITH_DEPRECATION_WARNINGS)
  hpx_add_config_define_namespace(
    DEFINE HPX_LOGGING_HAVE_DEPRECATION_WARNINGS
    NAMESPACE LOGGING)
endif()

# Added in 1.4.0
hpx_option(HPX_LOGGING_WITH_COMPATIBILITY_HEADERS
  BOOL
  "Enable compatibility headers for old headers"
  ON ADVANCED
  CATEGORY "Modules")

if(HPX_LOGGING_WITH_COMPATIBILITY_HEADERS)
  hpx_add_config_define_namespace(
    DEFINE HPX_LOGGING_HAVE_COMPATIBILITY_HEADERS
    NAMESPACE LOGGING)
endif()

message(STATUS "logging: Configuring")

set(headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/writer/format_write.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/writer/named_write.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/detail/time_format_holder.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/detail/template.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/detail/error.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/detail/macros.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/detail/level.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/detail/fwd.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/detail/manipulator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/detail/logger.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/detail/format_write_detail.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/detail/cache_before_init.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format_fwd.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/logging.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/formatter/named_spacer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/formatter/high_precision_time.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/formatter/time_strf.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/formatter/spacer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/formatter/time.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/formatter/defaults.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/formatter/convert_format.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/formatter/thread_id.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/array.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/named_write.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/destination/named.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/destination/defaults.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/destination/convert_destination.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/destination/file.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/op_equal.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/optimize.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/logging/format/named_write_fwd.hpp
)

if(HPX_LOGGING_WITH_COMPATIBILITY_HEADERS)
  set(compat_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/writer/format_write.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/writer/named_write.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/detail/time_format_holder.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/detail/template.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/detail/error.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/detail/macros.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/detail/level.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/detail/fwd.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/detail/manipulator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/detail/logger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/detail/format_write_detail.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/detail/cache_before_init.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format_fwd.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/logging.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/formatter/named_spacer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/formatter/high_precision_time.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/formatter/time_strf.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/formatter/spacer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/formatter/time.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/formatter/defaults.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/formatter/convert_format.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/formatter/thread_id.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/array.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/named_write.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/destination/named.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/destination/defaults.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/destination/convert_destination.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/destination/file.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/op_equal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/optimize.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/logging/format/named_write_fwd.hpp
  )
endif()

set(sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.cpp
)

add_library(hpx_logging STATIC ${sources} ${headers} ${compat_headers})

target_link_libraries(hpx_logging hpx_assertion hpx_config)
target_include_directories(hpx_logging PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

if(HPX_LOGGING_WITH_COMPATIBILITY_HEADERS)
  target_include_directories(hpx_logging PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility>)
endif()

target_compile_definitions(hpx_logging PUBLIC
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

if(HPX_LOGGING_WITH_COMPATIBILITY_HEADERS)
  add_hpx_source_group(
    NAME hpx
    ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    CLASS "Header Files"
    TARGETS ${compat_headers})
endif()

install(TARGETS hpx_logging EXPORT HPXTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT config
)
hpx_export_targets(hpx_logging)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT config)

if(HPX_LOGGING_WITH_COMPATIBILITY_HEADERS)
  install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT logging)
endif()

write_config_defines_file(
  NAMESPACE LOGGING
  FILENAME "${CMAKE_BINARY_DIR}/hpx/logging/config/defines.hpp")

write_config_defines_file(
  NAMESPACE LOGGING
  FILENAME "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpx/logging/config/defines.hpp")

add_subdirectory(examples)
add_subdirectory(tests)

message(STATUS "logging: Configuring done")
