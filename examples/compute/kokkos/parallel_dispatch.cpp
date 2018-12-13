#include <hpx/hpx_main.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/include/iostreams.hpp>

int main(int, char**)
{
    namespace kokkos = hpx::compute::kokkos;

    kokkos::parallel_for(kokkos::RangePolicy(0, 10),
        KOKKOS_LAMBDA(const int i) { hpx::cout << "i = " << i << hpx::endl; });

    kokkos::parallel_for(
        kokkos::RangePolicy<kokkos::DefaultExecutionSpace>(0, 10),
        KOKKOS_LAMBDA(const int i) { hpx::cout << "i = " << i << hpx::endl; });

    kokkos::parallel_for(
        kokkos::RangePolicy<kokkos::DefaultHostExecutionSpace>(0, 10),
        KOKKOS_LAMBDA(const int i) { hpx::cout << "i = " << i << hpx::endl; });

    int x = 0;
    kokkos::parallel_reduce(kokkos::RangePolicy<>(0, 10),
        KOKKOS_LAMBDA(const int i, int& update) { update += i; }, x);

    hpx::cout << "x = " << x << hpx::endl;

    return 0;
}
