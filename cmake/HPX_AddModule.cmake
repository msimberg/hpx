# Copyright (c) 2019 Auriane Reverdell
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(HPX_ExportTargets)

function(add_hpx_module libname modulename)
  # Retrieve arguments
  set(options DEPRECATION_WARNINGS CUDA CONFIG_FILES)
  # Compatibility needs to be on/off to allow 3 states : ON/OFF and disabled
  set(one_value_args COMPATIBILITY_HEADERS GLOBAL_HEADER_GEN)
  set(multi_value_args
      SOURCES
      HEADERS
      COMPAT_HEADERS
      DEPENDENCIES
      MODULE_DEPENDENCIES
      CMAKE_SUBDIRS
      EXCLUDE_FROM_GLOBAL_HEADER
  )
  cmake_parse_arguments(
    ${modulename} "${options}" "${one_value_args}" "${multi_value_args}"
    ${ARGN}
  )

  include(HPX_Message)
  include(HPX_Option)

  if(NOT "${${modulename}_COMPATIBILITY_HEADERS}" STREQUAL "")
    set(_have_compatibility_headers_option TRUE)
  else()
    set(_have_compatibility_headers_option FALSE)
  endif()

  # Global headers should be always generated except if explicitly disabled
  if("${${modulename}_GLOBAL_HEADER_GEN}" STREQUAL "")
    set(${modulename}_GLOBAL_HEADER_GEN ON)
  endif()

  string(TOUPPER ${libname} libname_upper)
  string(TOUPPER ${modulename} modulename_upper)

  # Mark the module as enabled (see hpx/modules/CMakeLists.txt)
  set(HPX_ENABLED_MODULES
        ${HPX_ENABLED_MODULES} ${modulename}
        CACHE INTERNAL "List of enabled HPX modules" FORCE
  )

  # HPX options
  hpx_option(
    HPX_${modulename_upper}_WITH_TESTS BOOL
    "Build HPX ${modulename} module tests. (default: ${HPX_WITH_TESTS})"
    ${HPX_WITH_TESTS} ADVANCED CATEGORY "Modules"
  )

  set(_deprecation_warnings_default OFF)
  if(${HPX_WITH_DEPRECATION_WARNINGS} AND ${modulename}_DEPRECATION_WARNINGS)
    set(_deprecation_warnings_default ON)
  endif()
  hpx_option(
    HPX_${modulename_upper}_WITH_DEPRECATION_WARNINGS
    BOOL
    "Enable warnings for deprecated facilities. (default: ${HPX_WITH_DEPRECATION_WARNINGS})"
    ${_deprecation_warnings_default}
    ADVANCED
    CATEGORY "Modules"
  )
  if(${HPX_${modulename_upper}_WITH_DEPRECATION_WARNINGS})
    hpx_add_config_define_namespace(
      DEFINE HPX_${modulename_upper}_HAVE_DEPRECATION_WARNINGS
      NAMESPACE ${modulename_upper}
    )
  endif()

  if(${_have_compatibility_headers_option})
    set(_compatibility_headers_default OFF)
    if(${modulename}_COMPATIBILITY_HEADERS)
      set(_compatibility_headers_default ON)
    endif()
    hpx_option(
      HPX_${modulename_upper}_WITH_COMPATIBILITY_HEADERS
      BOOL
      "Enable compatibility headers for old headers. (default: ${_compatibility_headers_default})"
      ${_compatibility_headers_default}
      ADVANCED
      CATEGORY "Modules"
    )
    if(HPX_${modulename_upper}_WITH_COMPATIBILITY_HEADERS)
      hpx_add_config_define_namespace(
        DEFINE HPX_${modulename_upper}_HAVE_COMPATIBILITY_HEADERS
        NAMESPACE ${modulename_upper}
      )
    endif()
  endif()

  # Main directories of the module
  set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")
  set(HEADER_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/include")

  hpx_debug("Add module ${modulename}: SOURCE_ROOT: ${SOURCE_ROOT}")
  hpx_debug("Add module ${modulename}: HEADER_ROOT: ${HEADER_ROOT}")

  if(${_have_compatibility_headers_option}
     AND HPX_${modulename_upper}_WITH_COMPATIBILITY_HEADERS
  )
    set(COMPAT_HEADER_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/include_compatibility")
    hpx_debug(
      "Add module ${modulename}: COMPAT_HEADER_ROOT: ${COMPAT_HEADER_ROOT}"
    )
  endif()

  # Write full path for the sources files
  list(
    TRANSFORM ${modulename}_SOURCES
    PREPEND ${SOURCE_ROOT}/
            OUTPUT_VARIABLE
            sources
  )
  list(
    TRANSFORM ${modulename}_HEADERS
    PREPEND ${HEADER_ROOT}/
            OUTPUT_VARIABLE
            headers
  )
  if(${_have_compatibility_headers_option}
     AND HPX_${modulename_upper}_WITH_COMPATIBILITY_HEADERS
  )
    list(
      TRANSFORM ${modulename}_COMPAT_HEADERS
      PREPEND ${COMPAT_HEADER_ROOT}/
              OUTPUT_VARIABLE
              compat_headers
    )
  endif()

  # This header generation is disabled for config module specific generated
  # headers are included
  if(${modulename}_GLOBAL_HEADER_GEN)
    # Add a global include file that include all module headers
    set(global_header
        "${CMAKE_CURRENT_BINARY_DIR}/include/hpx/${modulename}.hpp"
    )
    set(module_headers)
    foreach(header_file ${${modulename}_HEADERS})
      # Exclude the files specified
      if((NOT (${header_file} IN_LIST ${modulename}_EXCLUDE_FROM_GLOBAL_HEADER))
         AND (NOT ("${header_file}" MATCHES "detail"))
         AND NOT ("${header_file}" MATCHES "force_linking.hpp$")
      )
        set(module_headers "${module_headers}#include <${header_file}>\n")
      endif()
    endforeach(header_file)
    configure_file(
      "${PROJECT_SOURCE_DIR}/cmake/templates/global_module_header.hpp.in"
      "${global_header}"
    )
    set(generated_headers ${global_header})
  endif()

  set(config_entries_source
      "${CMAKE_CURRENT_BINARY_DIR}/src/config_entries.cpp"
  )

  # generate configuration header for this module
  set(config_header
      "${CMAKE_CURRENT_BINARY_DIR}/include/hpx/${modulename}/config/defines.hpp"
  )
  write_config_defines_file(
    NAMESPACE ${modulename_upper} FILENAME ${config_header}
  )
  set(generated_headers ${generated_headers} ${config_header})

  if(${modulename}_CONFIG_FILES)
    # Version file
    set(global_config_file
        ${CMAKE_CURRENT_BINARY_DIR}/include/hpx/config/version.hpp
    )
    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/config_version.hpp.in"
      "${global_config_file}" @ONLY
    )
    set(generated_headers ${generated_headers} ${global_config_file})
    # Global config defines file (different from the one for each module)
    set(global_config_file
        ${CMAKE_CURRENT_BINARY_DIR}/include/hpx/config/defines.hpp
    )
    write_config_defines_file(
      TEMPLATE
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/config_defines.hpp.in"
      NAMESPACE default
      FILENAME "${global_config_file}"
    )
    set(generated_headers ${generated_headers} ${global_config_file})
  endif()

  # list all specified headers
  foreach(header_file ${headers})
    hpx_debug(${header_file})
  endforeach(header_file)

  # create library modules
  # cmake-format: off
  add_library(
    hpx_${modulename} OBJECT
    ${sources} ${config_entries_source}
  )
  # cmake-format: on

  target_link_libraries(
    hpx_${modulename} PUBLIC ${${modulename}_MODULE_DEPENDENCIES}
  )
  target_link_libraries(hpx_${modulename} PUBLIC ${${modulename}_DEPENDENCIES})
  target_include_directories(
    hpx_${modulename}
    PUBLIC $<BUILD_INTERFACE:${HEADER_ROOT}>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
           $<INSTALL_INTERFACE:include>
  )

  target_link_libraries(
    hpx_${modulename}
    PUBLIC hpx_public_flags
    PRIVATE hpx_private_flags
    PUBLIC hpx_base_libraries
  )

  # All modules depend on the config registry
  if(NOT "${modulename}" STREQUAL "config_registry")
    target_link_libraries(hpx_${modulename} PUBLIC hpx_config_registry)
  endif()

  if(${_have_compatibility_headers_option}
     AND HPX_${modulename_upper}_WITH_COMPATIBILITY_HEADERS
  )
    target_include_directories(
      hpx_${modulename} PUBLIC $<BUILD_INTERFACE:${COMPAT_HEADER_ROOT}>
    )
  endif()

  target_compile_definitions(
    hpx_${modulename}
    PRIVATE $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Debug>:_DEBUG>
            HPX_${libname_upper}_EXPORTS
  )

  # This is a temporary solution until all of HPX has been modularized as it
  # enables using header files from HPX for compiling this module.
  target_include_directories(hpx_${modulename} PRIVATE ${HPX_SOURCE_DIR})

  add_hpx_source_group(
    NAME hpx_{modulename}
    ROOT ${HEADER_ROOT}/hpx
    CLASS "Header Files"
    TARGETS ${headers}
  )
  add_hpx_source_group(
    NAME hpx_{modulename}
    ROOT ${SOURCE_ROOT}
    CLASS "Source Files"
    TARGETS ${sources}
  )
  if(${_have_compatibility_headers_option}
     AND HPX_${modulename_upper}_WITH_COMPATIBILITY_HEADERS
  )
    add_hpx_source_group(
      NAME hpx_${modulename}
      ROOT ${COMPAT_HEADER_ROOT}/hpx
      CLASS "Header Files"
      TARGETS ${compat_headers}
    )
  endif()

  if(${modulename}_GLOBAL_HEADER_GEN OR ${modulename}_CONFIG_FILES)
    add_hpx_source_group(
      NAME hpx_{modulename}
      ROOT ${CMAKE_CURRENT_BINARY_DIR}/include/hpx
      CLASS "Generated Files"
      TARGETS ${generated_headers}
    )
  endif()
  add_hpx_source_group(
    NAME hpx_{modulename}
    ROOT ${CMAKE_CURRENT_BINARY_DIR}/include/hpx
    CLASS "Generated Files"
    TARGETS ${config_header}
  )

  set_target_properties(
    hpx_${modulename} PROPERTIES FOLDER "Core/Modules" POSITION_INDEPENDENT_CODE
                                                       ON
  )

  # Add a static library that only contains the object library.
  add_library(hpx_${modulename}_static $<TARGET_OBJECTS:hpx_${modulename}>)
  target_link_libraries(hpx_${modulename}_static PUBLIC hpx_${modulename})

  foreach(lib ${${modulename}_MODULE_DEPENDENCIES})
    target_link_libraries(hpx_${modulename}_static PUBLIC ${lib}_static)
  endforeach()

  add_hpx_pseudo_dependencies(core hpx_${modulename}_static)

  if(MSVC)
    set_target_properties(
      hpx_${modulename}
      PROPERTIES COMPILE_PDB_NAME_DEBUG hpx_${modulename}d
                 COMPILE_PDB_NAME_RELWITHDEBINFO hpx_${modulename}
                 COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG
                 ${CMAKE_CURRENT_BINARY_DIR}/Debug
                 COMPILE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO
                 ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo
    )
  endif()

  # Install the static library for the module
  install(
    TARGETS hpx_${modulename}_static
    EXPORT HPXInternalTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${modulename}
  )
  install(
    TARGETS hpx_${modulename}
    EXPORT HPXInternalTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${modulename}
  )
  hpx_export_internal_targets(hpx_${modulename} hpx_${modulename}_static)

  # Install the headers from the source
  install(
    DIRECTORY include/hpx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT ${modulename}
  )

  # Install the compatibility headers from the source
  if(${_have_compatibility_headers_option}
     AND HPX_${modulename_upper}_WITH_COMPATIBILITY_HEADERS
  )
    install(
      DIRECTORY include_compatibility/hpx
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      COMPONENT ${modulename}
    )
  endif()

  # Installing the generated header files from the build dir
  install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/hpx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT ${modulename}
  )

  # install PDB if needed
  if(MSVC)
    foreach(cfg DEBUG;RELWITHDEBINFO)
      get_target_property(_pdb_file hpx_${modulename} COMPILE_PDB_NAME_${cfg})
      get_target_property(
        _pdb_dir hpx_${modulename} COMPILE_PDB_OUTPUT_DIRECTORY_${cfg}
      )
      install(
        FILES ${_pdb_dir}/${_pdb_file}.pdb
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        CONFIGURATIONS ${cfg}
        OPTIONAL
      )
    endforeach()
  endif()

  # Link modules to their higher-level libraries
  target_link_libraries(hpx_${libname} PUBLIC hpx_${modulename})

  # We only do this for libraries other than hpx_full
  if(NOT ${libname} STREQUAL "full")
    target_link_libraries(hpx_${libname}_static PUBLIC hpx_${modulename}_static)
  endif()

  foreach(dir ${${modulename}_CMAKE_SUBDIRS})
    add_subdirectory(${dir})
  endforeach(dir)

  include(HPX_PrintSummary)
  create_configuration_summary(
    "  Module configuration summary (${modulename}):" "${modulename}"
  )

endfunction(add_hpx_module)
