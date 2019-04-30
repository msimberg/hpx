# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

project(HPX.util CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HPX_AddDefinitions)
include(HPX_Option)

hpx_option(HPX_UTIL_WITH_TESTS
  BOOL
  "Build HPX util module tests. (default: ON)"
  ON ADVANCED
  CATEGORY "Modules")

message(STATUS "util: Configuring")

set(headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/detail/pack.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/allocator_deleter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/always_void.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/assert_owns_lock.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/atomic_count.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/await_traits.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/calculate_fanout.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/decay.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/detected.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/fibhash.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/get_and_reset_value.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/identity.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/insert_checked.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/internal_allocator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/iterator_adaptor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/iterator_facade.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/iterator_range.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/jenkins_hash.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/lazy_conditional.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/lazy_enable_if.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/manage_config.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/max.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/memory_chunk.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/min.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/optional.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/range.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/safe_lexical_cast.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/set_thread_name.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/static.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/thread_specific_ptr.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/unlock_guard.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/unused.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/unwrap_ref.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/util/void_guard.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/detail/reserve.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/detail/wrap_int.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/concepts.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/has_member_xxx.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/has_xxx.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/is_iterator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/is_range.hpp
)

set(sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/manage_config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/set_thread_name.cpp
)

add_library(hpx_util STATIC ${sources} ${headers})

target_link_libraries(hpx_util hpx_config)
target_include_directories(hpx_util PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

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

set_target_properties(hpx_util PROPERTIES
  FOLDER "Core/Modules")

install(TARGETS hpx_util EXPORT HPXTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT util
)
hpx_export_targets(hpx_util)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT util)

add_subdirectory(examples)
add_subdirectory(tests)

message(STATUS "util: Configuring done")
