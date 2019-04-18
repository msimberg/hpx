# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

project(HPX.hardware CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HPX_AddDefinitions)
include(HPX_Option)

hpx_option(HPX_HARDWARE_WITH_TESTS
  BOOL
  "Build HPX hardware module tests. (default: ON)"
  ON ADVANCED
  CATEGORY "Modules")

hpx_option(HPX_HARDWARE_WITH_DEPRECATION_WARNINGS
  BOOL
  "Enable warnings for deprecated facilities. (default: ${HPX_WITH_DEPRECATION_WARNINGS})"
  ${HPX_WITH_DEPRECATION_WARNINGS} ADVANCED
  CATEGORY "Modules")
if(HPX_HARDWARE_WITH_DEPRECATION_WARNINGS)
  hpx_add_config_define_namespace(
    DEFINE HPX_HARDWARE_HAVE_DEPRECATION_WARNINGS
    NAMESPACE HARDWARE)
endif()

# Added in 1.4.0
hpx_option(HPX_HARDWARE_WITH_COMPATIBILITY_HEADERS
  BOOL
  "Enable compatibility headers for old headers"
  ON ADVANCED
  CATEGORY "Modules")

if(HPX_HARDWARE_WITH_COMPATIBILITY_HEADERS)
  hpx_add_config_define_namespace(
    DEFINE HPX_HARDWARE_HAVE_COMPATIBILITY_HEADERS
    NAMESPACE HARDWARE)
endif()

message(STATUS "hardware: Configuring")

set(headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/hardware/cpuid/linux_x86.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/hardware/cpuid/msvc.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/hardware/timestamp/bgq.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/hardware/timestamp/linux_generic.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/hardware/timestamp/linux_x86_32.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/hardware/timestamp/linux_x86_64.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/hardware/timestamp/msvc.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/hardware/bit_manipulation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/hardware/cpuid.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/hardware/timestamp.hpp)

if(HPX_HARDWARE_WITH_COMPATIBILITY_HEADERS)
  set(compat_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/hardware/cpuid/linux_x86.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/hardware/cpuid/msvc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/hardware/timestamp/bgq.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/hardware/timestamp/linux_generic.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/hardware/timestamp/linux_x86_32.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/hardware/timestamp/linux_x86_64.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/hardware/timestamp/msvc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/hardware/bit_manipulation.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/hardware/cpuid.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/hardware/timestamp.hpp
  )
endif()

# cmake will not create a separate VS project without any source files, thus
# this adds a dummy (empty) source file to the target
set(sources ${CMAKE_CURRENT_SOURCE_DIR}/hardware.cpp)

add_library(hpx_hardware STATIC ${headers} ${compat_headers})

target_link_libraries(hpx_hardware hpx_config)
target_include_directories(hpx_hardware INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(HPX_HARDWARE_WITH_COMPATIBILITY_HEADERS)
  target_include_directories(hpx_hardware PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility>)
endif()

include(HPX_AddSourceGroup)
add_hpx_source_group(
  NAME hpx
  ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  CLASS "Header Files"
  TARGETS ${headers})
add_hpx_source_group(
  NAME hpx_hardware
  ROOT ${CMAKE_CURRENT_SOURCE_DIR}
  CLASS "Source Files"
  TARGETS ${sources})

if(HPX_HARDWARE_WITH_COMPATIBILITY_HEADERS)
  add_hpx_source_group(
    NAME hpx
    ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    CLASS "Header Files"
    TARGETS ${compat_headers})
endif()

install(TARGETS hpx_hardware EXPORT HPXTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT hardware
)
hpx_export_targets(hpx_hardware)

install(
  DIRECTORY include/hpx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT hardware)

if(HPX_HARDWARE_WITH_COMPATIBILITY_HEADERS)
  install(
    DIRECTORY include_compatibility/hpx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT hardware)
endif()

write_config_defines_file(
  NAMESPACE HARDWARE
  FILENAME "${CMAKE_BINARY_DIR}/hpx/hardware/config/defines.hpp")

write_config_defines_file(
  NAMESPACE HARDWARE
  FILENAME "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpx/hardware/config/defines.hpp")

add_subdirectory(examples)
add_subdirectory(tests)

message(STATUS "hardware: Configuring done")
