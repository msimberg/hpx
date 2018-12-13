#include <hpx/include/iostreams.hpp>
#include <Kokkos_Core.hpp>

namespace hpx { namespace compute { namespace kokkos {
    using namespace Kokkos;
}}}

namespace hpx { namespace compute { namespace kokkos {
    struct default_executor
    {
        using execution_space = Kokkos::DefaultExecutionSpace;
    };

    struct host_executor
    {
        using execution_space = Kokkos::DefaultHostExecutionSpace;
    };

    template <typename KokkosExecutor,
        typename B,
        typename E,
        typename S,
        typename F>
    void for_loop_custom(KokkosExecutor&& exec,
        B first,
        E last,
        S stride,
        F&& f)
    {
        HPX_ASSERT_MSG(
            stride == 1, "for_loop with Kokkos executor can only use stride 1");
        std::cout << "in customized hpx::parallel::detail::for_loop_custom"
                  << std::endl;

        kokkos::parallel_for(
            // TODO: Want to get execution space from executor.
            // kokkos::RangePolicy<typename KokkosExecutor::execution_space>(first, last),
            kokkos::RangePolicy(first, last),
            f);
        kokkos::fence();
    }

    void customizable_for_loop(default_executor)
    {
        std::cout << "using hpx::compute::kokkos::customizable_for_loop"
                  << std::endl;
    }
}}}

namespace hpx { namespace parallel { namespace execution {
    // TODO: This is not true, but makes templates work out.
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
    struct is_one_way_executor<compute::kokkos::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_one_way_executor<compute::kokkos::host_executor> : std::true_type
    {
    };

    template <>
    struct is_two_way_executor<compute::kokkos::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_two_way_executor<compute::kokkos::host_executor> : std::true_type
    {
    };

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
    struct is_bulk_two_way_executor<compute::kokkos::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_bulk_two_way_executor<compute::kokkos::host_executor>
      : std::true_type
    {
    };
}}}
