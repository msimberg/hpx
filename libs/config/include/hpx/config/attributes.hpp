//  Copyright (c) 2017 Marcin Copik
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_CONFIG_ATTRIBUTES_HPP
#define HPX_CONFIG_ATTRIBUTES_HPP

#include <hpx/config/compiler_specific.hpp>
#include <hpx/config/defines.hpp>

#if defined(DOXYGEN)

/// Function attribute to tell compiler not to inline the function.
#define HPX_NOINLINE
/// Function attribute to tell compiler that the function does not return.
#define HPX_NORETURN
/// Marks an entity as deprecated. The argument \c x specifies a custom message
/// that is included in the compiler warning. For more details see
/// `<https://en.cppreference.com/w/cpp/language/attributes/deprecated>`__.
#define HPX_DEPRECATED(x)
/// Indicates that the fall through from the previous case label is intentional
/// and should not be diagnosed by a compiler that warns on fallthrough. For
/// more details see
/// `<https://en.cppreference.com/w/cpp/language/attributes/fallthrough>`__.
#define HPX_FALLTHROUGH
#else

///////////////////////////////////////////////////////////////////////////////
// clang-format off
#if defined(HPX_MSVC)
#   define HPX_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#   if defined(__NVCC__) || defined(__CUDACC__)
        // nvcc doesn't always parse __noinline
#       define HPX_NOINLINE __attribute__ ((noinline))
#   else
#       define HPX_NOINLINE __attribute__ ((__noinline__))
#   endif
#else
#   define HPX_NOINLINE
#endif

///////////////////////////////////////////////////////////////////////////////
// handle [[noreturn]]
#if defined(HPX_HAVE_CXX11_NORETURN_ATTRIBUTE)
#   define HPX_NORETURN [[noreturn]]
#else
#  if defined(_MSC_VER)
#    define HPX_NORETURN __declspec(noreturn)
#  elif defined(__GNUC__)
#    define HPX_NORETURN __attribute__ ((__noreturn__))
#  else
#    define HPX_NORETURN
#  endif
#endif

///////////////////////////////////////////////////////////////////////////////
// handle [[deprecated]]
#if defined(HPX_HAVE_DEPRECATION_WARNINGS)
#  define HPX_DEPRECATED_MSG \
   "This functionality is deprecated and will be removed in the future."
#  if defined(HPX_HAVE_CXX14_DEPRECATED_ATTRIBUTE)
#    define HPX_DEPRECATED(x) [[deprecated(x)]]
#  elif defined(HPX_MSVC)
#    define HPX_DEPRECATED(x) __declspec(deprecated(x))
#  elif defined(__GNUC__)
#    define HPX_DEPRECATED(x) __attribute__((__deprecated__(x)))
#  endif
#endif

#if !defined(HPX_DEPRECATED)
#  define HPX_DEPRECATED(x)
#endif

///////////////////////////////////////////////////////////////////////////////
// handle [[fallthrough]]
#if defined(HPX_HAVE_CXX17_FALLTHROUGH_ATTRIBUTE)
#   define HPX_FALLTHROUGH [[fallthrough]]
#elif defined(HPX_GCC_VERSION) && (HPX_GCC_VERSION >= 70000) && \
     !defined(HPX_INTEL_VERSION)
#   define HPX_FALLTHROUGH __attribute__((fallthrough))
// All versions of clang supported by HPX have the [[clang::fallthrough]]
// attribute.
#elif defined(HPX_CLANG_VERSION)
#   define HPX_FALLTHROUGH [[clang::fallthrough]]
#else
#   define HPX_FALLTHROUGH
#endif
// clang-format on

#endif
#endif
