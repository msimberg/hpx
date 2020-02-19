//  Copyright (c) 2017 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file hpx/runtime/thread_pool_helpers.hpp

#ifndef HPX_RUNTIME_THREAD_POOL_HELPERS_HPP
#define HPX_RUNTIME_THREAD_POOL_HELPERS_HPP

#include <hpx/config.hpp>
#include <hpx/threading_base/thread_pool_base.hpp>

#include <cstddef>
#include <string>

namespace hpx { namespace resource
{
    ///////////////////////////////////////////////////////////////////////////
    /// Return the number of thread pools currently managed by the
    /// \a resource_partitioner
    HPX_API_EXPORT std::size_t get_num_thread_pools();

    /// Return the number of threads in all thread pools currently
    /// managed by the \a resource_partitioner
    HPX_API_EXPORT std::size_t get_num_threads();

    /// Return the number of threads in the given thread pool currently
    /// managed by the \a resource_partitioner
    HPX_API_EXPORT std::size_t get_num_threads(std::string const& pool_name);

    /// Return the number of threads in the given thread pool currently
    /// managed by the \a resource_partitioner
    HPX_API_EXPORT std::size_t get_num_threads(std::size_t pool_index);

    /// Return the internal index of the pool given its name.
    HPX_API_EXPORT std::size_t get_pool_index(std::string const& pool_name);

    /// Return the name of the pool given its internal index
    HPX_API_EXPORT std::string const& get_pool_name(std::size_t pool_index);

    /// Return the name of the pool given its name
    HPX_API_EXPORT threads::thread_pool_base& get_thread_pool(
        std::string const& pool_name);

    /// Return the thread pool given its internal index
    HPX_API_EXPORT threads::thread_pool_base& get_thread_pool(
        std::size_t pool_index);
}}

namespace hpx { namespace threads {
    ///    The function \a get_thread_count returns the number of currently
    /// known threads.
    ///
    /// \param state    [in] This specifies the thread-state for which the
    ///                 number of threads should be retrieved.
    ///
    /// \note If state == unknown this function will not only return the
    ///       number of currently existing threads, but will add the number
    ///       of registered task descriptions (which have not been
    ///       converted into threads yet).
    HPX_API_EXPORT std::int64_t get_thread_count(
        thread_state_enum state = unknown);

    /// The function \a get_thread_count returns the number of currently
    /// known threads.
    ///
    /// \param priority [in] This specifies the thread-priority for which the
    ///                 number of threads should be retrieved.
    /// \param state    [in] This specifies the thread-state for which the
    ///                 number of threads should be retrieved.
    ///
    /// \note If state == unknown this function will not only return the
    ///       number of currently existing threads, but will add the number
    ///       of registered task descriptions (which have not been
    ///       converted into threads yet).
    HPX_API_EXPORT std::int64_t get_thread_count(
        thread_priority priority, thread_state_enum state = unknown);

    /// The function \a enumerate_threads will invoke the given function \a f
    /// for each thread with a matching thread state.
    ///
    /// \param f        [in] The function which should be called for each
    ///                 matching thread. Returning 'false' from this function
    ///                 will stop the enumeration process.
    /// \param state    [in] This specifies the thread-state for which the
    ///                 threads should be enumerated.
    HPX_API_EXPORT bool enumerate_threads(
        util::function_nonser<bool(thread_id_type)> const& f,
        thread_state_enum state = unknown);
}}

#endif
