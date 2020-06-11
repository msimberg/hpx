//  Copyright (c) 2007-2016 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/executors/detail/fork_join_executor.hpp>    // TODO: Move out of detail
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/testing.hpp>
#include <hpx/timing.hpp>

#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

static std::atomic<std::size_t> count{0};

///////////////////////////////////////////////////////////////////////////////
void bulk_test(int value, int passed_through)    //-V813
{
    ++count;
    HPX_TEST_EQ(passed_through, 42);
}

void test_bulk_sync()
{
    using executor = hpx::parallel::execution::experimental::fork_join_executor;

    hpx::thread::id tid = hpx::this_thread::get_id();

    std::size_t const n = 13;
    std::vector<int> v(n);
    std::iota(std::begin(v), std::end(v), std::rand());

    using hpx::util::placeholders::_1;
    using hpx::util::placeholders::_2;

    hpx::util::high_resolution_timer t;
    executor exec;
    double t0 = t.elapsed();
    hpx::parallel::execution::bulk_sync_execute(
        exec, hpx::util::bind(&bulk_test, _1, _2), v, 42);
    double t1 = t.elapsed();
    HPX_TEST_EQ(count.load(), n);

    hpx::parallel::execution::bulk_sync_execute(exec, &bulk_test, v, 42);
    double t2 = t.elapsed();
    HPX_TEST_EQ(count.load(), 2 * n);

    printf("%f\n%f\n%f\n", t0, t1, t2);
}

void static_check_executor()
{
    using namespace hpx::traits;
    using executor = hpx::parallel::execution::experimental::fork_join_executor;

    static_assert(!has_sync_execute_member<executor>::value,
        "!has_sync_execute_member<executor>::value");
    static_assert(!has_async_execute_member<executor>::value,
        "!has_async_execute_member<executor>::value");
    static_assert(!has_then_execute_member<executor>::value,
        "!has_then_execute_member<executor>::value");
    static_assert(has_bulk_sync_execute_member<executor>::value,
        "has_bulk_sync_execute_member<executor>::value");
    static_assert(has_bulk_async_execute_member<executor>::value,
        "has_bulk_async_execute_member<executor>::value");
    static_assert(!has_bulk_then_execute_member<executor>::value,
        "!has_bulk_then_execute_member<executor>::value");
    static_assert(
        !has_post_member<executor>::value, "!has_post_member<executor>::value");
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main(int argc, char* argv[])
{
    static_check_executor();

    test_bulk_sync();

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // By default this test should run on all available cores
    std::vector<std::string> const cfg = {"hpx.os_threads=all"};

    // Initialize and run HPX
    HPX_TEST_EQ_MSG(
        hpx::init(argc, argv, cfg), 0, "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
