#include <hpx/hpx_init.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/parallel_for_each.hpp>
#include <hpx/include/parallel_for_loop.hpp>
#include <hpx/include/parallel_reduce.hpp>
#include <hpx/include/sync.hpp>
#include <hpx/util/lightweight_test.hpp>

#include <boost/range/irange.hpp>

#include <sched.h>
#include <omp.h>

int n = 1000;

int main(int argc, char** argv)
{
    std::vector<int> v(n, 0);

    hpx::util::high_resolution_timer t;

#pragma omp parallel // num_threads(2)
    {
        omp_set_num_threads(2);

#pragma omp parallel for
        for (int i = 0; i < 10; ++i)
        {
            printf("thread = %d/%d, cpu = %d, i = %d\n", omp_get_thread_num(),
                   omp_get_num_threads(), sched_getcpu(), i);
        }
    }

    double time = t.elapsed();
}
