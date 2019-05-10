# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

project(HPX.concurrency CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HPX_AddDefinitions)
include(HPX_Option)

hpx_option(HPX_CONCURRENCY_WITH_TESTS
  BOOL
  "Build HPX concurrency module tests. (default: ON)"
  ON ADVANCED
  CATEGORY "Modules")

hpx_option(HPX_CONCURRENCY_WITH_DEPRECATION_WARNINGS
  BOOL
  "Enable warnings for deprecated facilities. (default: ${HPX_WITH_DEPRECATION_WARNINGS})"
  ${HPX_WITH_DEPRECATION_WARNINGS} ADVANCED
  CATEGORY "Modules")
if(HPX_CONCURRENCY_WITH_DEPRECATION_WARNINGS)
  hpx_add_config_define_namespace(
    DEFINE HPX_CONCURRENCY_HAVE_DEPRECATION_WARNINGS
    NAMESPACE CONCURRENCY)
endif()

# Added in 1.4.0
hpx_option(HPX_CONCURRENCY_WITH_COMPATIBILITY_HEADERS
  BOOL
  "Enable compatibility headers for old headers"
  ON ADVANCED
  CATEGORY "Modules")

if(HPX_CONCURRENCY_WITH_COMPATIBILITY_HEADERS)
  hpx_add_config_define_namespace(
    DEFINE HPX_CONCURRENCY_HAVE_COMPATIBILITY_HEADERS
    NAMESPACE CONCURRENCY)
endif()

message(STATUS "concurrency: Configuring")

set(headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/concurrency.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/concurrency/barrier.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/concurrency/deque.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/concurrency/detail/freelist.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/concurrency/detail/tagged_ptr_pair.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/concurrency/itt_notify.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/concurrency/register_locks.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/concurrency/spinlock.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/concurrency/spinlock_pool.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/concurrency/thread_name.hpp
)

if(HPX_CONCURRENCY_WITH_COMPATIBILITY_HEADERS)
  set(compat_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/compat/barrier.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/lockfree/deque.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/lockfree/detail/tagged_ptr_pair.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/lockfree/freelist.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/itt_notify.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/register_locks.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/spinlock.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/spinlock_pool.hpp
  )
endif()

set(sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/barrier.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/itt_notify.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/register_locks.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/thread_name.cpp
)

add_library(hpx_concurrency STATIC ${sources} ${headers} ${compat_headers})

target_link_libraries(hpx_concurrency hpx_assertion hpx_config hpx_errors hpx_util)
target_include_directories(hpx_concurrency PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

if(HPX_CONCURRENCY_WITH_COMPATIBILITY_HEADERS)
  target_include_directories(hpx_concurrency PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility>)
endif()

target_compile_definitions(hpx_concurrency PUBLIC
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

if(HPX_CONCURRENCY_WITH_COMPATIBILITY_HEADERS)
  add_hpx_source_group(
    NAME hpx
    ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    CLASS "Header Files"
    TARGETS ${compat_headers})
endif()

install(TARGETS hpx_concurrency EXPORT HPXTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT config
)
hpx_export_targets(hpx_concurrency)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT config)

if(HPX_CONCURRENCY_WITH_COMPATIBILITY_HEADERS)
  install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT concurrency)
endif()

write_config_defines_file(
  NAMESPACE CONCURRENCY
  FILENAME "${CMAKE_BINARY_DIR}/hpx/concurrency/config/defines.hpp")

write_config_defines_file(
  NAMESPACE CONCURRENCY
  FILENAME "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpx/concurrency/config/defines.hpp")

add_subdirectory(examples)
add_subdirectory(tests)

message(STATUS "concurrency: Configuring done")
