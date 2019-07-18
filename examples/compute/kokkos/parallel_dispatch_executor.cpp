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

// This example uses the Kokkos executors (which satisfy HPX executor
// requirements). They forward work to corresponding Kokkos functions. If needed
// they will fall back back to HPX functions or synchronous, sequential
// execution.

int n = 1000;

int hpx_main()
{
    namespace kokkos = hpx::compute::kokkos;
    kokkos::runtime_guard k;

    {
        // Test post and async_execute
        std::cout << "*** basic executor tests" << std::endl;
        hpx::sync(kokkos::default_executor(),
            []() { std::cout << "hello from sync\n"; });
        hpx::apply(kokkos::default_executor(),
            []() { std::cout << "hello from apply\n"; });
        auto f = hpx::async(kokkos::host_executor(), []() {
            std::cout << "hello from async\n";
            return 1;
        });
        HPX_TEST(f.get() == 1);
    }

    {
        // for_each is not overloaded for kokkos executors, should fall back on bulk_(a)sync_execute
        std::cout << "*** for_each test" << std::endl;
        std::vector<int> v(5, 0);
        std::vector<int> v_reference(v);

        auto f_for_each = [](int& x) {
            std::cout << "setting x = 13\n";
            x = 13;
        };

        auto f_for_each_reference = [](int& x) {
            std::cout << "*setting x = 13\n";
            x = 13;
        };

        hpx::parallel::for_each(
            hpx::parallel::execution::par.on(kokkos::default_executor()),
            std::begin(v), std::end(v), f_for_each);

        hpx::parallel::for_each(hpx::parallel::execution::seq,
            std::begin(v_reference), std::end(v_reference),
            f_for_each_reference);

        HPX_TEST(v == v_reference);

        // TODO: What type is this?
        auto s = hpx::parallel::for_each(
            hpx::parallel::execution::par.on(kokkos::default_executor()),
            std::begin(v), std::end(v), [](int& x) { return x + 1; });
    }

    {
        // for_loop is overloaded for kokkos executors
        std::cout << "*** for_loop test" << std::endl;
        std::vector<int> v(5, 0);
        std::vector<int> v_reference(v);

        auto f_for_loop = [&v](const int i) { v[i] = i; };

        auto f_for_loop_reference = [&v_reference](
                                        const int i) { v_reference[i] = i; };

        std::cout
            << "is_base_of<kokkos_executor_base, default_executor>::value = "
            << std::is_base_of<
                   hpx::compute::kokkos::detail::kokkos_executor_base<
                       kokkos::DefaultExecutionSpace>,
                   hpx::compute::kokkos::default_executor>::value
            << std::endl;

        {
            kokkos::is_kokkos_executor<kokkos::default_executor> a{};
        }
        {
            kokkos::is_kokkos_executor<kokkos::host_executor> a{};
        }
        {
            kokkos::is_kokkos_executor<
                hpx::parallel::execution::parallel_executor>
                a{};
        }
        std::cout << "is_kokkos_executor<default_executor>::value = "
                  << kokkos::is_kokkos_executor<kokkos::default_executor>::value
                  << std::endl;

        std::cout << "is_kokkos_executor<host_executor>::value = "
                  << kokkos::is_kokkos_executor<kokkos::host_executor>::value
                  << std::endl;

        std::cout << "is_kokkos_executor<parallel_executor>::value = "
                  << kokkos::is_kokkos_executor<
                         hpx::parallel::execution::parallel_executor>::value
                  << std::endl;

        // This is overloaded
        hpx::parallel::for_loop(
            hpx::parallel::execution::par.on(kokkos::default_executor()), 0, 5,
            f_for_loop);
        // This is not overloaded, and not allowed (seq.on(parallel))
        // hpx::parallel::for_loop(
        //     hpx::parallel::execution::seq.on(kokkos::default_executor()), 0, 5,
        //     f_for_loop);
        // This is excluded from the overload and falls back on bulk_*_execute
        hpx::parallel::for_loop(
            hpx::parallel::execution::par.on(kokkos::default_executor()),
            std::begin(v), std::end(v), [](std::vector<int>::iterator x) {
                std::cout << "got x = " << *x << std::endl;
            });
        // This is not overloaded, and falls back on sync execute
        // This does not work for some reason
        // hpx::parallel::for_loop(
        //     hpx::parallel::execution::seq.on(kokkos::serial_executor()), 0, 5,
        //     f_for_loop);
        hpx::parallel::for_loop(
            hpx::parallel::execution::seq, 0, 5, f_for_loop_reference);
        HPX_ASSERT(v == v_reference);
    }

    {
        // reduce is overloaded for kokkos executors
        std::cout << "*** reduce test" << std::endl;
        auto range = boost::irange(0, 4);
        std::plus<int> f_reduce_plus{};
        auto result = hpx::parallel::reduce(
            hpx::parallel::execution::par.on(kokkos::default_executor()),
            std::begin(range), std::end(range), 3, f_reduce_plus);
        auto result_ref = hpx::parallel::reduce(hpx::parallel::execution::seq,
            std::begin(range), std::end(range), 3, [](int a, int b) {
                std::cout << "a = " << a << ", b = " << b << std::endl;
                return a + b;
            });
        HPX_ASSERT(result == result_ref);
    }

    // {
    //     auto range = boost::irange(1, 5);
    //     std::multiplies<int> f_reduce_multiplies{};
    //     auto result = hpx::parallel::reduce(kokkos::serial_executor(),
    //         std::begin(range), std::end(range), 3, f_reduce_multiplies);
    //     auto result_ref = hpx::parallel::reduce(hpx::parallel::execution::seq,
    //         std::begin(range), std::end(range), 3, f_reduce_multiplies);
    //     HPX_ASSERT(result == result_ref);
    // }

    // {
    //     auto exec = hpx::compute::openmp::default_executor();

    //     std::vector<int> v(n, 0);
    //     std::vector<int> v_reference(n, 1);

    //     auto f_for_loop = [](std::vector<int>& v) {
    //         return [&v](const int i) { v[i] = i * i / std::sin(i); };
    //     };

    //     std::cout << "openmp num threads: " << exec.processing_units_count()
    //               << std::endl;

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
    // }

    std::cout << "shutting down" << std::endl;

    kokkos::finalize();

    return hpx::finalize();
}

int main(int argc, char** argv)
{
    hpx::init(argc, argv);

    std::vector<int> v(n, 0);

    hpx::util::high_resolution_timer t;

#pragma omp parallel for
    for (int i = 0; i < n; ++i)
    {
        v[i] = i * i / std::sin(i);
    }

    double omp_time = t.elapsed();
    std::cout << "omp raw time: " << omp_time << " s\n";

    for (int i = 0; i < n; ++i)
    {
        v[i] = i * i / std::sin(i);
    }

    double seq_time = t.elapsed();
    std::cout << "seq raw time: " << seq_time << " s\n";
}
