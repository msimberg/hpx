//  Copyright (c) 2019 Mikael Simberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_COMPUTE_OPENMP_DEFAULT_EXECUTOR_HPP
#define HPX_COMPUTE_OPENMP_DEFAULT_EXECUTOR_HPP

#include <hpx/config.hpp>

#include <hpx/lcos/future.hpp>
#include <hpx/parallel/executors/execution.hpp>
#include <hpx/parallel/executors/static_chunk_size.hpp>
#include <hpx/throw_exception.hpp>
#include <hpx/traits/executor_traits.hpp>
#include <hpx/traits/is_executor.hpp>
#include <hpx/traits/is_iterator.hpp>
#include <hpx/traits/is_range.hpp>
#include <hpx/util/decay.hpp>
#include <hpx/util/invoke.hpp>
#include <hpx/util/range.hpp>
#include <hpx/util/tuple.hpp>

#include <omp.h>

#include <algorithm>
#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace hpx { namespace compute { namespace openmp {

    // NOTE: This is not actually needed. The default can simply be changed.
    // struct default_executor_parameters
    // {
    //     template <typename Executor, typename F>
    //     std::size_t get_chunk_size(Executor& exec,
    //         F&&,
    //         std::size_t cores,
    //         std::size_t num_tasks)
    //     {
    //         return std::size_t(-1);
    //     }
    // };

    // An OpenMP executor only does something special for bulk async. All
    // operations are blocking (NOTE: it could be made to return futures if
    // operations are executed e.g. on the main thread). Operations that return
    // futures, block first and then return ready futures.
    struct default_executor
    {
        // using executor_parameters_type = default_executor_parameters;
        using executor_parameters_type = hpx::parallel::execution::static_chunk_size;

        default_executor() {}

        std::size_t processing_units_count() const
        {
            std::size_t count = omp_get_max_threads();

            return count;
        }

        template <typename F, typename... Ts>
        void post(F&& f, Ts&&... ts) const
        {
            hpx::util::invoke(f, ts...);
        }

        template <typename F, typename... Ts>
        hpx::future<void> async_execute(F&& f, Ts&&... ts) const
        {
            f(ts...);
            return hpx::make_ready_future<void>();
        }

        template <typename F, typename... Ts>
        void sync_execute(F&& f, Ts&&... ts) const
        {
            f(ts...);
        }

        template <typename F, typename Shape, typename... Ts>
        void bulk_launch(F&& f, Shape const& shape, Ts&&... ts) const
        {
            bulk_sync_execute(
                std::forward<F>(f), shape, std::forward<Ts>(ts)...);
        }

        template <typename F, typename Shape, typename... Ts>
        std::vector<hpx::future<void>> bulk_async_execute(F&& f,
            Shape const& shape,
            Ts&&... ts) const
        {
            bulk_sync_execute(
                std::forward<F>(f), shape, std::forward<Ts>(ts)...);
            return std::vector<hpx::future<void>>();
        }

        template <typename F, typename Shape, typename... Ts>
        void bulk_sync_execute(F&& f, Shape const& shape, Ts&&... ts) const
        {
            using value_type =
                typename hpx::traits::range_traits<Shape>::value_type;

            std::vector<value_type> const chunks(
                hpx::util::begin(shape), hpx::util::end(shape));

            std::cout << "openmp_executor::bulk_sync_execute: have " << chunks.size() << " chunks\n";

#pragma omp parallel for
            for (int i = 0; i < static_cast<int>(chunks.size()); ++i)
            {
                f(chunks[i], ts...);
            }
        }
    };
}}}

namespace hpx { namespace parallel { namespace execution {
    // template <>
    // struct is_executor_parameters<compute::openmp::default_executor_parameters>
    //   : std::true_type
    // {
    // };

    template <>
    struct executor_execution_category<compute::openmp::default_executor>
    {
        typedef parallel::execution::parallel_execution_tag type;
    };

    template <>
    struct is_one_way_executor<compute::openmp::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_two_way_executor<compute::openmp::default_executor>
      : std::false_type
    {
    };

    template <>
    struct is_bulk_one_way_executor<compute::openmp::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_bulk_two_way_executor<compute::openmp::default_executor>
      : std::false_type
    {
    };
}}}

#endif
