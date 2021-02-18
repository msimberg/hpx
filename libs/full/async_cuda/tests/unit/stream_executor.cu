//  Copyright (c) 2021 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/init.hpp>
#include <hpx/modules/async_cuda.hpp>
#include <hpx/modules/execution.hpp>
#include <hpx/modules/executors.hpp>
#include <hpx/modules/testing.hpp>

__global__ void dummy_kernel()
{
    printf("Hello from device kernel.\n");
}

struct dummy
{
    void operator()() const
    {
        std::cout << "Hello from host kernel." << std::endl;
    }

    void operator()(cudaStream_t stream) const
    {
        dummy_kernel<<<1, 1, 0, stream>>>();
    }
};

int hpx_main()
{
    using namespace hpx::cuda::experimental;
    using namespace hpx::execution::experimental;

    // Pure stream executor
    {
        std::cerr << "Scheduling no transforms" << std::endl;
        sync_wait(schedule(stream_executor{cudaStream_t{}}));
        std::cerr << "Waited for results" << std::endl;
    }

    {
        std::cerr << "Scheduling one transform" << std::endl;
        sync_wait(
            transform(schedule(stream_executor{cudaStream_t{}}), dummy{}));
        std::cerr << "Waited for results" << std::endl;
    }

    {
        std::cerr << "Scheduling three transforms" << std::endl;
        auto s1 = transform(schedule(stream_executor{cudaStream_t{}}), dummy{});
        auto s2 = transform(s1, dummy{});
        sync_wait(s2);
        std::cerr << "Waited for results" << std::endl;
    }

    // Mixing stream executor with host executor
    {
        std::cerr << "Scheduling no transforms" << std::endl;
        auto s1 = schedule(stream_executor{cudaStream_t{}});
        auto s2 = on(s1, executor{});
        auto s3 = on(s2, stream_executor{cudaStream_t{}});
        sync_wait(s3);
        std::cerr << "Waited for results" << std::endl;
    }

    {
        std::cerr << "Scheduling no transforms" << std::endl;
        auto s1 = schedule(executor{});
        auto s2 = on(s1, stream_executor{cudaStream_t{}});
        auto s3 = on(s2, executor{});
        sync_wait(s3);
        std::cerr << "Waited for results" << std::endl;
    }

    {
        std::cerr << "Scheduling one transform on each context" << std::endl;
        auto s1 = schedule(stream_executor{cudaStream_t{}});
        auto s2 = transform(s1, dummy{});
        auto s3 = on(s2, executor{});
        auto s4 = transform(s3, dummy{});
        auto s5 = on(s4, stream_executor{cudaStream_t{}});
        auto s6 = transform(s5, dummy{});
        sync_wait(s6);
        std::cerr << "Waited for results" << std::endl;
    }

    {
        std::cerr << "Scheduling one transform on each context" << std::endl;
        auto s1 = schedule(executor{});
        auto s2 = transform(s1, dummy{});
        auto s3 = on(s2, stream_executor{cudaStream_t{}});
        auto s4 = transform(s3, dummy{});
        auto s5 = on(s4, executor{});
        auto s6 = transform(s5, dummy{});
        sync_wait(s6);
        std::cerr << "Waited for results" << std::endl;
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    HPX_TEST_EQ_MSG(
        hpx::init(argc, argv), 0, "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
