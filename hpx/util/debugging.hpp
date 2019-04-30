//  Copyright (c) 2007-2012 Hartmut Kaiser
//  Copyright (c) 2017      Denis Blank
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_UTIL_DEBUGGING_HPP
#define HPX_UTIL_DEBUGGING_HPP

#include <hpx/config.hpp>

namespace hpx {
namespace util {
    /// Tries to break an attached debugger, if not supported a loop is
    /// invoked which gives enough time to attach a debugger manually.
    void HPX_EXPORT attach_debugger();
    /// Attaches a debugger if \c category is equal to the configuration entry
    /// hpx.attach-debugger.
    void HPX_EXPORT may_attach_debugger(std::string category);
}    // end namespace util
}    // end namespace hpx

#endif    // HPX_UTIL_DEBUGGING_HPP
