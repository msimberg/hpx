#include <hip/hip_runtime_api.h>
#include <array>

__host__ __device__ void test() {
    std::array<int, 1> arr;
    // operator[] produces a const reference in device code, when it should
    // produce a non-const reference!
    static_assert(std::is_same<int&, decltype(arr[1])>::value,
        "accessing non-const array should give non-const reference");
}

int main(int argc, char* argv[])
{
    test();
    return 0;
}
