#include <hpx/include/iostreams.hpp>
#include <thrust/for_each.h>
#include <thrust/host_vector.h>
#include <thrust/system/omp/execution_policy.h>

namespace hpx { namespace compute { namespace thrust {
    using namespace ::thrust;
}}}

namespace hpx { namespace compute { namespace thrust {
    // implementation.

    struct default_executor
    {
    };
}}}

namespace hpx { namespace parallel {
    template <typename I, typename F,
        HPX_CONCEPT_REQUIRES_(
            (hpx::traits::is_iterator<I>::value || std::is_integral<I>::value))>
    void for_loop(hpx::compute::thrust::default_executor exec,
        typename std::decay<I>::type first, I last, F&& f)
    {
        std::cout << "in thrust::for_loop" << std::endl;
        hpx::compute::thrust::for_each(
            hpx::compute::thrust::omp::par, first, last, f);
    }

    template <typename FwdIter, typename T, typename F>
    T reduce(hpx::compute::thrust::default_executor policy, FwdIter first,
        FwdIter last, T init, F&& f)
    {
        hpx::compute::thrust::reduce(
            hpx::compute::thrust::omp::par, first, last, init, f);

        return init;
    }
}}

namespace hpx { namespace parallel { namespace execution {
    // TODO: This is not true, but makes templates work out.
    template <>
    struct executor_execution_category<compute::thrust::default_executor>
    {
        typedef parallel::execution::parallel_execution_tag type;
    };

    template <>
    struct is_one_way_executor<compute::thrust::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_two_way_executor<compute::thrust::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_bulk_one_way_executor<compute::thrust::default_executor>
      : std::true_type
    {
    };

    template <>
    struct is_bulk_two_way_executor<compute::thrust::default_executor>
      : std::true_type
    {
    };
}}}
