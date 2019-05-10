# Copyright (c) 2019 The STE||AR-Group
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

project(HPX.functional CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(HPX_AddDefinitions)
include(HPX_Option)

hpx_option(HPX_FUNCTIONAL_WITH_TESTS
  BOOL
  "Build HPX functional module tests. (default: ON)"
  ON ADVANCED
  CATEGORY "Modules")

hpx_option(HPX_FUNCTIONAL_WITH_DEPRECATION_WARNINGS
  BOOL
  "Enable warnings for deprecated facilities. (default: ${HPX_WITH_DEPRECATION_WARNINGS})"
  ${HPX_WITH_DEPRECATION_WARNINGS} ADVANCED
  CATEGORY "Modules")
if(HPX_FUNCTIONAL_WITH_DEPRECATION_WARNINGS)
  hpx_add_config_define_namespace(
    DEFINE HPX_FUNCTIONAL_HAVE_DEPRECATION_WARNINGS
    NAMESPACE FUNCTIONAL)
endif()

# Added in 1.4.0
hpx_option(HPX_FUNCTIONAL_WITH_COMPATIBILITY_HEADERS
  BOOL
  "Enable compatibility headers for old headers"
  ON ADVANCED
  CATEGORY "Modules")

if(HPX_FUNCTIONAL_WITH_COMPATIBILITY_HEADERS)
  hpx_add_config_define_namespace(
    DEFINE HPX_FUNCTIONAL_HAVE_COMPATIBILITY_HEADERS
    NAMESPACE FUNCTIONAL)
endif()

message(STATUS "functional: Configuring")

set(headers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/detail/vtable/callable_vtable.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/detail/vtable/copyable_vtable.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/detail/vtable/function_vtable.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/detail/basic_function.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/detail/empty_function.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/detail/function_registration.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/detail/reset_function.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/bind.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/bind_back.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/bind_front.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/function.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/function_ref.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/invoke.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/invoke_fused.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/mem_fn.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/one_shot.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/protect.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/result_of.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional/unique_function.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/get_function_address.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/get_function_annotation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/is_action.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/is_bind_expression.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/is_callable.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/traits/is_placeholder.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx/functional.hpp
)

if(HPX_FUNCTIONAL_WITH_COMPATIBILITY_HEADERS)
  set(compat_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/bind.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/bind_back.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/bind_front.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/function.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/function_ref.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/invoke.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/invoke_fused.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/mem_fn.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/one_shot.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/protect.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/result_of.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx/util/unique_function.hpp
  )
endif()

set(sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/basic_function.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/empty_function.cpp
)

add_library(hpx_functional STATIC ${sources} ${compat_headers})

target_link_libraries(hpx_functional hpx_assertion hpx_concurrency hpx_config hpx_errors)
target_include_directories(hpx_functional PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

if(HPX_FUNCTIONAL_WITH_COMPATIBILITY_HEADERS)
  target_include_directories(hpx_functional PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility>)
endif()

target_compile_definitions(hpx_functional PUBLIC
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

if(HPX_FUNCTIONAL_WITH_COMPATIBILITY_HEADERS)
  add_hpx_source_group(
    NAME hpx
    ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    CLASS "Header Files"
    TARGETS ${compat_headers})
endif()

install(TARGETS hpx_functional EXPORT HPXTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT config
)
hpx_export_targets(hpx_functional)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/hpx
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT config)

if(HPX_FUNCTIONAL_WITH_COMPATIBILITY_HEADERS)
  install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility/hpx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT functional)
endif()

write_config_defines_file(
  NAMESPACE FUNCTIONAL
  FILENAME "${CMAKE_BINARY_DIR}/hpx/functional/config/defines.hpp")

write_config_defines_file(
  NAMESPACE FUNCTIONAL
  FILENAME "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hpx/functional/config/defines.hpp")

add_subdirectory(examples)
add_subdirectory(tests)

message(STATUS "functional: Configuring done")
