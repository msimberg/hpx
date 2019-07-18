#include <hpx/include/iostreams.hpp>
#include <hpx/parallel/executors/static_chunk_size.hpp>
#include <hpx/parallel/util/projection_identity.hpp>
#include <Kokkos_Core.hpp>

#include <type_traits>

namespace hpx { namespace parallel { namespace execution {
    template <typename F, typename Shape, typename... Ts>
    struct bulk_function_result;
}}}

namespace hpx { namespace compute { namespace kokkos {
    // Bring in all public Kokkos functionality into hpx::compute::kokkos.
    using namespace Kokkos;
}}}

namespace hpx { namespace compute { namespace kokkos {
    // Define a base struct for dispatching to parallel algorithms. We use the
    // execution_space typedef to for dispatching to the correct Kokkos
    // implementation.

    struct runtime_guard
    {
        template <typename... Args>
        runtime_guard(Args&&... args)
        {
            initialize(std::forward<Args>(args)...);
        }

        ~runtime_guard()
        {
            finalize();
        }
    };

    namespace detail {
        template <typename T>
        struct kokkos_executor_base
        {
            using execution_space = T;

            /// \cond NOINTERNAL
            bool operator==(kokkos_executor_base<execution_space> const&) const
                noexcept
            {
                return true;
            }

            bool operator!=(kokkos_executor_base<execution_space> const&) const
                noexcept
            {
                return false;
            }

            kokkos_executor_base<execution_space> const& context() const
                noexcept
            {
                return *this;
            }
            /// \endcond

            using executor_parameters_type =
                hpx::parallel::execution::static_chunk_size;

            // default_executor() {}

            std::size_t processing_units_count() const
            {
                return execution_space::concurrency();
            }

            // This is one-way, single-task execution. This cheats and blocks.
            template <typename F, typename... Ts>
            void post(F&& f, Ts&&... ts) const
            {
                std::cout << "kokkos_executor_base::post\n";
                // NOTE: Alternatively, fall back to HPX functions (i.e. spawn
                // an actual task instead of running inline). When post gets
                // called one doesn't expect the performance of Kokkos anyway,
                // because it's for one-off tasks.
                sync_execute(std::forward<F>(f), std::forward<Ts>(ts)...);
            }

            template <typename F, typename... Ts>
            hpx::future<typename hpx::util::detail::invoke_deferred_result<F,
                Ts...>::type>
            async_execute(F&& f, Ts&&... ts) const
            {
                std::cout << "kokkos_executor_base::async_execute\n";
                // NOTE: Alternatively, fall back to HPX functions. When post
                // gets called one doesn't expect the performance of Kokkos
                // anyway, because it's for one-off tasks.
                return hpx::make_ready_future<>(
                    sync_execute(std::forward<F>(f), std::forward<Ts>(ts)...));
            }

            template <typename F, typename... Ts>
            typename hpx::util::detail::invoke_deferred_result<F, Ts...>::type
            sync_execute(F&& f, Ts&&... ts) const
            {
                std::cout << "kokkos_executor_base::sync_execute\n";
                return hpx::util::invoke(
                    std::forward<F>(f), std::forward<Ts>(ts)...);
            }

            template <typename F, typename Shape, typename... Ts>
            std::vector<hpx::future<typename parallel::execution::detail::
                    bulk_function_result<F, Shape, Ts...>::type>>
            bulk_async_execute(F&& f, Shape const& shape, Ts&&... ts) const
            {
                std::cout << "kokkos_executor_base::bulk_async_execute\n";

                using result_type = std::vector<
                    hpx::future<typename hpx::parallel::execution::detail::
                            bulk_function_result<F, Shape, Ts...>::type>>;

                result_type results;
                std::size_t size = hpx::util::size(shape);
                results.resize(size);

                using value_type =
                    typename hpx::traits::range_traits<Shape>::value_type;

                std::vector<value_type> const chunks(
                    hpx::util::begin(shape), hpx::util::end(shape));

                for (int i = 0; i < static_cast<int>(size); ++i)
                {
                    //     results.push_back(hpx::make_ready_future<>(
                    //         hpx::util::invoke(std::forward<F>(f), chunks[i],
                    //             std::forward<Ts>(ts)...)));
                    hpx::util::invoke(
                        std::forward<F>(f), chunks[i], std::forward<Ts>(ts)...);
                }

                return results;
            }

            template <typename F, typename Shape, typename... Ts>
            typename parallel::execution::detail::bulk_execute_result<F, Shape,
                Ts...>::type
            bulk_sync_execute(F&& f, Shape const& shape, Ts&&... ts) const
            {
                std::cout << "kokkos_executor_base::bulk_sync_execute\n";
                typename parallel::execution::detail::bulk_execute_result<F,
                    Shape, Ts...>::type results;
                std::size_t size = hpx::util::size(shape);
                results.reserve(size);

                using value_type =
                    typename hpx::traits::range_traits<Shape>::value_type;

                std::vector<value_type> const chunks(
                    hpx::util::begin(shape), hpx::util::end(shape));

                hpx::compute::kokkos::parallel_for(
                    hpx::compute::kokkos::RangePolicy<
                        typename Executor::execution_space>(0, size),
                    KOKKOS_LAMBDA(I i) {
                        std::cout << "calling chunk function with "
                                     "argument i = "
                                  << i << std::endl;
                        hpx::util::invoke(std::forward<F>(f), chunks[i],
                            std::forward<Ts>(ts)...);
                    });

                hpx::compute::kokkos::fence();

                return hpx::make_ready_future<void>();
            }
        };
    }

    template <typename Executor, typename = void>
    struct is_kokkos_executor : std::false_type
    {
    };

    template <typename Executor>
    struct is_kokkos_executor<Executor,
        typename std::enable_if<std::is_base_of<
            detail::kokkos_executor_base<typename Executor::execution_space>,
            Executor>::value>::type> : std::true_type
    {
    };

    // These execution spaces are always available.
    struct default_executor
      : detail::kokkos_executor_base<kokkos::DefaultExecutionSpace>
    {
    };

    struct host_executor
      : detail::kokkos_executor_base<kokkos::DefaultHostExecutionSpace>
    {
    };

    // The following execution spaces are only conditionally enabled in Kokkos.
#if defined(KOKKOS_ENABLE_SERIAL)
    struct serial_executor : detail::kokkos_executor_base<kokkos::Serial>
    {
    };
#endif

#if defined(KOKKOS_ENABLE_HPX)
    struct hpx_executor : detail::kokkos_executor_base<kokkos::HPX>
    {
    };
#endif

#if defined(KOKKOS_ENABLE_OPENMP)
    struct openmp_executor : detail::kokkos_executor_base<kokkos::OpenMP>
    {
    };
#endif

#if defined(KOKKOS_ENABLE_CUDA)
    struct cuda_executor : detail::kokkos_executor_base<kokkos::CUDA>
    {
    };
#endif

#if defined(KOKKOS_ENABLE_ROCM)
    struct rocm_executor : detail::kokkos_executor_base<kokkos::ROCm>
    {
    };
#endif
}}}

namespace hpx { namespace parallel {
    template <typename Executor, typename Parameters, typename I, typename F,
        HPX_CONCEPT_REQUIRES_((!hpx::traits::is_iterator<I>::value &&
                                  std::is_integral<I>::value) &&
            compute::kokkos::is_kokkos_executor<Executor>::value)>
    void for_loop(execution::parallel_policy_shim<Executor, Parameters> exec,
        typename std::decay<I>::type first, I last, F&& f)
    {
        static_assert(std::is_integral<I>::value,
            "for_loop overload for Kokkos executor can only be used with "
            "integral ranges. Use HPX executors or rewrite your range to "
            "use "
            "integral types.");

        std::cout << "customized for_loop for kokkos_executor_base"
                  << std::endl;
        hpx::compute::kokkos::parallel_for(
            hpx::compute::kokkos::RangePolicy<
                typename Executor::execution_space>(first, last),
            KOKKOS_LAMBDA(I i) {
                std::cout << "calling user provided function with argument i = "
                          << i << std::endl;
                f(i);
            });
        hpx::compute::kokkos::fence();
    }

    template <typename Executor, typename Parameters, typename FwdIter,
        typename T, typename F,
        HPX_CONCEPT_REQUIRES_((hpx::traits::is_iterator<FwdIter>::value) &&
            compute::kokkos::is_kokkos_executor<Executor>::value)>
    inline typename util::detail::algorithm_result<
        hpx::parallel::execution::parallel_policy_shim<Executor, Parameters>,
        T>::type
    reduce(hpx::parallel::execution::parallel_policy_shim<Executor, Parameters>
               policy,
        FwdIter first, FwdIter last, T init, F&& f)
    {
        std::cout << "customized reduce for kokkos_executor_base" << std::endl;

        hpx::compute::kokkos::parallel_reduce(
            hpx::compute::kokkos::RangePolicy<
                typename Executor::execution_space>(*first, *last),
            KOKKOS_LAMBDA(
                typename hpx::compute::kokkos::RangePolicy<
                    typename Executor::execution_space>::member_type i,
                T & update) {
                std::cout << "calling user provided function with "
                             "arguments i = "
                          << i << " and update = " << update << std::endl;
                update = f(i, update);
            },
            init);
        hpx::compute::kokkos::fence();

        return init;
    }

}}

namespace hpx { namespace parallel { namespace execution {
    template <>
    struct executor_execution_category<compute::kokkos::default_executor>
    {
        typedef parallel::execution::parallel_execution_tag type;
    };

    template <>
    struct executor_execution_category<compute::kokkos::host_executor>
    {
        typedef parallel::execution::parallel_execution_tag type;
    };

    template <>
    struct executor_execution_category<compute::kokkos::serial_executor>
    {
        typedef parallel::execution::sequenced_execution_tag type;
    };

    template <>
    struct is_one_way_executor<compute::kokkos::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_one_way_executor<compute::kokkos::host_executor> : std::true_type
    {
    };

    // NOTE: Kokkos executors don't return futures, thus no two-way
    // template <>
    // struct is_two_way_executor<compute::kokkos::default_executor>
    //   : std::true_type
    // {
    // };

    // template <>
    // struct is_two_way_executor<compute::kokkos::host_executor> : std::true_type
    // {
    // };

    // template <>
    // struct is_two_way_executor<compute::kokkos::serial_executor>
    //   : std::true_type
    // {
    // };

    template <>
    struct is_bulk_one_way_executor<compute::kokkos::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_bulk_one_way_executor<compute::kokkos::host_executor>
      : std::true_type
    {
    };

    template <>
    struct is_bulk_one_way_executor<compute::kokkos::serial_executor>
      : std::true_type
    {
    };

    // template <>
    // struct is_bulk_two_way_executor<compute::kokkos::default_executor>
    //   : std::true_type
    // {
    // };

    // template <>
    // struct is_bulk_two_way_executor<compute::kokkos::host_executor>
    //   : std::true_type
    // {
    // };

    // template <>
    // struct is_bulk_two_way_executor<compute::kokkos::serial_executor>
    //   : std::true_type
    // {
    // };
}}}
