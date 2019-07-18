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

#include <omp.h>
#include <sched.h>

#include <vector>

int n = 1000;

int hpx_main()
{
    {
        auto exec = hpx::compute::openmp::default_executor();

        std::vector<int> v(n, 0);
        std::vector<int> v_reference(n, 1);

        std::cout << "openmp num threads: " << exec.processing_units_count()
                  << std::endl;

        hpx::parallel::for_loop(
            hpx::parallel::execution::par.on(exec), 0, 10, [](const int i) {
                printf("thread = %d/%d, cpu = %d, i = %d\n",
                    omp_get_thread_num(), omp_get_num_threads(), sched_getcpu(),
                    i);
            });

        //     hpx::util::high_resolution_timer t;
        //     hpx::parallel::for_loop(
        //         hpx::parallel::execution::seq, 0, n, [&v_reference](const int i) {
        //             v_reference[i] = i * i / std::sin(i);
        //         });
        //     double seq_time = t.elapsed();
        //     std::cout << "seq time: " << seq_time << " s\n";

        //     t.restart();
        //     hpx::parallel::for_loop(hpx::parallel::execution::par, 0, n,
        //         [&v](const int i) { v[i] = i * i / std::sin(i); });
        //     double par_time = t.elapsed();
        //     std::cout << "par time: " << par_time << " s\n";

        //     t.restart();
        //     hpx::parallel::for_loop(hpx::parallel::execution::par, 0, n,
        //         [&v](const int i) { v[i] = i * i / std::sin(i); });
        //     par_time = t.elapsed();
        //     std::cout << "par time: " << par_time << " s\n";

        //     t.restart();
        //     hpx::parallel::for_loop(hpx::parallel::execution::par, 0, n,
        //         [&v](const int i) { v[i] = i * i / std::sin(i); });
        //     par_time = t.elapsed();
        //     std::cout << "par time: " << par_time << " s\n";

        //     t.restart();
        //     hpx::parallel::for_loop(hpx::parallel::execution::par, 0, n,
        //         [&v](const int i) { v[i] = i * i / std::sin(i); });
        //     par_time = t.elapsed();
        //     std::cout << "par time: " << par_time << " s\n";

        //     t.restart();
        //     hpx::parallel::for_loop(hpx::parallel::execution::par.on(exec), 0, n,
        //         [&v](const int i) { v[i] = i * i / std::sin(i); });
        //     double omp_time = t.elapsed();
        //     std::cout << "omp time: " << omp_time << " s\n";

        //     t.restart();
        //     hpx::parallel::for_loop(hpx::parallel::execution::par.on(exec), 0, n,
        //         [&v](const int i) { v[i] = i * i / std::sin(i); });
        //     omp_time = t.elapsed();
        //     std::cout << "omp time: " << omp_time << " s\n";

        //     t.restart();
        //     hpx::parallel::for_loop(hpx::parallel::execution::par.on(exec), 0, n,
        //         [&v](const int i) { v[i] = i * i / std::sin(i); });
        //     omp_time = t.elapsed();
        //     std::cout << "omp time: " << omp_time << " s\n";

        //     {
        //         auto range = boost::irange(0, n);
        //         t.restart();
        //         hpx::parallel::for_loop(::hpx::compute::thrust::default_executor(),
        //             range.begin(), range.end(),
        //             [&v](const int i) { v[i] = i * i / std::sin(i); });
        //         double thrust_omp_time = t.elapsed();
        //         std::cout << "thrust_omp time: " << thrust_omp_time << " s\n";
        //     }

        //     HPX_ASSERT(v == v_reference);
    }

    std::cout << "shutting down" << std::endl;

    return hpx::finalize();
}

int main(int argc, char** argv)
{
    hpx::init(argc, argv);
}
