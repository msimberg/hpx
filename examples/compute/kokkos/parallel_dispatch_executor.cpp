#include <hpx/hpx_main.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/parallel_for_loop.hpp>

int main(int, char**)
{
    namespace kokkos = hpx::compute::kokkos;

    // hpx::parallel::for_loop(kokkos::default_executor(), 0, 10,
    //     [](const int i) { hpx::cout << "i = " << i << hpx::endl; });

    // hpx::parallel::customizable_for_loop(kokkos::default_executor());
    // hpx::parallel::customizable_for_loop(hpx::parallel::execution::par);

    // hpx::parallel::customizable_for_loop_2(kokkos::default_executor());
    // hpx::parallel::customizable_for_loop_2(hpx::parallel::execution::par);

    // hpx::parallel::customizable_for_loop_full(
    //     kokkos::default_executor(), 3, [](int i) { return 5 * i; });
    // hpx::parallel::customizable_for_loop_full(
    //     hpx::parallel::execution::par, 5, [](int i) { return 17 * i; });

    // hpx::parallel::for_loop_custom(kokkos::default_executor(), 0, 10,
    //     [](int i) { std::cout << "* i = " << i << std::endl; });
    // hpx::parallel::for_loop_custom(hpx::parallel::execution::par, 0, 10,
    //     [](int i) { std::cout << "i = " << i << std::endl; });

    // hpx::parallel::customizable_for_loop(kokkos::default_executor());
    // hpx::parallel::customizable_for_loop(hpx::parallel::execution::par);

    auto f = [](const int i) {
        std::cout << "parallel for lambda, i = " << i << std::endl;
    };

    hpx::parallel::for_loop_custom(kokkos::default_executor(), 0, 3, f);
    hpx::parallel::for_loop_custom(kokkos::host_executor(), 0, 4, f);
    hpx::parallel::for_loop_custom(hpx::parallel::execution::par.on(kokkos::host_executor()), 0, 4, f);
    hpx::parallel::for_loop_custom(hpx::parallel::execution::par, 0, 5, f);

    //     hpx::parallel::for_loop(kokkos::host_executor(), 0, 10,
    //         [](const int i) { hpx::cout << "i = " << i << hpx::endl; });
    //
    //     hpx::parallel::for_loop(kokkos::device_executor(), 0, 10,
    //         [](const int i) { hpx::cout << "i = " << i << hpx::endl; });
    //
    //     hpx::parallel::for_loop(kokkos::device_executor(), 0, 10,
    //         [](const int i, kokkos::sub_executor sub_exec) {
    //             hpx::parallel::for_loop(sub_exec, 0, 10, [](const int j, blah) {
    //                 // Can't use hpx::cout (or std::cout) on CUDA.
    //                 hpx::cout << "i = " << i << ", j = " << j << hpx::endl;
    //             });
    //         });
    //
    //     hpx::parallel::reduce(kokkos::default_executor(), 0, 10,
    //         [](const int i) { hpx::cout << "i = " << i << hpx::endl; });
    //
    //     hpx::parallel::scan(kokkos::default_executor(), 0, 10,
    //         [](const int i) { hpx::cout << "i = " << i << hpx::endl; });
    //
    //     // Should fall back on host executor if not available.
    //     hpx::parallel::transform(kokkos::device_executor(), 0, 10,
    //         [](const int i) { hpx::cout << "i = " << i << hpx::endl; });
    //
    //     hpx::parallel::sort(kokkos::device_executor(), 0, 10,
    //         [](const int i) { hpx::cout << "i = " << i << hpx::endl; });

    return 0;
}
