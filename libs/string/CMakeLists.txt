# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(string_headers
  hpx/string/detail/case_conv.hpp
  hpx/string/detail/classification.hpp
  hpx/string/detail/find_format.hpp
  hpx/string/detail/find_format_all.hpp
  hpx/string/detail/find_format_store.hpp
  hpx/string/detail/find_iterator.hpp
  hpx/string/detail/finder.hpp
  hpx/string/detail/formatter.hpp
  hpx/string/detail/predicate.hpp
  hpx/string/detail/replace_storage.hpp
  hpx/string/detail/sequence.hpp
  hpx/string/detail/trim.hpp
  hpx/string/detail/util.hpp
  hpx/string/case_conv.hpp
  hpx/string/classification.hpp
  hpx/string/compare.hpp
  hpx/string/concept.hpp
  hpx/string/constants.hpp
  hpx/string/erase.hpp
  hpx/string/find.hpp
  hpx/string/find_format.hpp
  hpx/string/find_iterator.hpp
  hpx/string/finder.hpp
  hpx/string/formatter.hpp
  hpx/string/iter_find.hpp
  hpx/string/join.hpp
  hpx/string/predicate.hpp
  hpx/string/predicate_facade.hpp
  hpx/string/replace.hpp
  hpx/string/sequence_traits.hpp
  hpx/string/split.hpp
  hpx/string/std/list_traits.hpp
  hpx/string/std/string_traits.hpp
  hpx/string/std_containers_traits.hpp
  hpx/string/trim.hpp
  hpx/string/trim_all.hpp
  hpx/string/yes_no_type.hpp
)

include(HPX_AddModule)
add_hpx_module(string
  FORCE_LINKING_GEN
  GLOBAL_HEADER_GEN ON
  HEADERS ${string_headers}
  DEPENDENCIES hpx_config
  CMAKE_SUBDIRS examples tests
)
