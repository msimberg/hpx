//  Copyright (c) 2012 Bryce Adelstein-Lelbach
//  Copyright (c) 2014 Agustin Berge
//  Copyright (c) 2017 Denis Blank
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/datastructures/tuple.hpp>
#include <hpx/futures/future.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/threads.hpp>
#include <hpx/modules/testing.hpp>
#include <hpx/pack_traversal/unwrap.hpp>

#include <array>
#include <atomic>
#include <cstddef>
#include <numeric>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using hpx::get;
using hpx::make_tuple;
using hpx::tuple;
using hpx::util::unwrap;

template <template <typename> class FutureType, typename FutureProvider>
void test_unwrapping(FutureProvider&& futurize)
{
    // Unwrap single tuple like types back.
    {
        std::array<FutureType<int>, 2> in{{futurize(1), futurize(2)}};

        std::array<int, 2> result = unwrap(in);

        HPX_TEST((result == std::array<int, 2>{{1, 2}}));
    }
}

template <template <typename> class FutureType>
struct future_factory
{
    FutureType<void> operator()() const
    {
        return hpx::lcos::make_ready_future();
    }

    template <typename T>
    FutureType<typename std::decay<T>::type> operator()(T&& value) const
    {
        return hpx::lcos::make_ready_future(std::forward<T>(value));
    }
};

template <template <typename> class FutureType>
void test_all()
{
    future_factory<FutureType> provider;
    test_unwrapping<FutureType>(provider);
}

int hpx_main()
{
    // Test everything using default futures
    test_all<hpx::lcos::future>();
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // Configure application-specific options.
    hpx::program_options::options_description cmdline(
        "usage: " HPX_APPLICATION_STRING " [options]");

    // We force this test to use several threads by default.
    std::vector<std::string> const cfg = {"hpx.os_threads=all"};

    // Initialize and run HPX
    if (int result = hpx::init(cmdline, argc, argv, cfg))
    {
        return result;
    }
    // Report errors after HPX was finished
    return hpx::util::report_errors();
}
