//  Copyright (c) 2017 Mikael Simberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Simple test verifying basic resource_partitioner functionality.

#include <hpx/compat/thread.hpp>
#include <hpx/hpx_start.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/resource_partitioner.hpp>
#include <hpx/include/threadmanager.hpp>
#include <hpx/include/threads.hpp>
#include <hpx/runtime/threads/executors/pool_executor.hpp>
#include <hpx/runtime/threads/policies/scheduler_mode.hpp>
#include <hpx/runtime/threads/policies/schedulers.hpp>
#include <hpx/runtime/threads/thread_helpers.hpp>
#include <hpx/util/bind.hpp>
#include <hpx/util/lightweight_test.hpp>

#include <atomic>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

int hpx_main(int, char**)
{
    std::cout << "hello from hpx_main!\n";

    // for (std::size_t i = 0; i < 20; ++i)
    // {
    //     hpx::this_thread::suspend(std::chrono::milliseconds(100));
    // }

    // std::cout << "hpx::finalize();\n";
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // std::vector<std::string> cfg =
    // {
    //     "hpx.os_threads=4"
    // };

    hpx::resource::partitioner rp(argc, argv); //, std::move(cfg));

    // rp.create_thread_pool("default",
    //     [](hpx::threads::policies::callback_notifier& notifier,
    //         std::size_t num_threads, std::size_t thread_offset,
    //         std::size_t pool_index, std::string const& pool_name)
    //     -> std::unique_ptr<hpx::threads::detail::thread_pool_base>
    //     {
    //         std::unique_ptr<hpx::threads::policies::local_priority_queue_scheduler<>>
    //             scheduler(new hpx::threads::policies::local_priority_queue_scheduler<>(num_threads));

    //         auto mode = hpx::threads::policies::scheduler_mode(
    //             // hpx::threads::policies::do_background_work |
    //             hpx::threads::policies::reduce_thread_priority |
    //             // hpx::threads::policies::delay_exit |
    //             hpx::threads::policies::enable_elasticity);

    //         std::unique_ptr<hpx::threads::detail::thread_pool_base> pool(
    //             new hpx::threads::detail::scheduled_thread_pool<hpx::threads::policies::local_priority_queue_scheduler<>>(
    //                 std::move(scheduler), notifier, pool_index, pool_name, mode,
    //                 thread_offset));

    //         return pool;
    //     });

    // HPX_TEST_EQ(hpx::init(argc, argv), 0);
    // std::cout << "hpx::start(argc, argv);\n";
    // hpx::start(argc, argv);
    // std::cout << "hpx::suspend();\n";
    // hpx::suspend();

    // std::cout << "hpx::resume();\n";
    // hpx::resume(f, argc, argv);
    // std::cout << "hpx::stop();\n";
    // hpx::stop();

    // for (std::size_t i = 0; i < 10; ++i)
    // {
    //     hpx::util::high_resolution_timer t;

    //     hpx::start(argc, argv);
    //     // hpx::async([]() { hpx::finalize(); });
    //     hpx::stop();

    //     double time = t.elapsed();
    //     std::cout << "init: " << time << " s\n";
    // }

    {
        rp.create_thread_pool("default",
                              [](hpx::threads::policies::callback_notifier& notifier,
                                 std::size_t num_threads, std::size_t thread_offset,
                                 std::size_t pool_index, std::string const& pool_name)
                              -> std::unique_ptr<hpx::threads::detail::thread_pool_base>
                              {
                                  std::unique_ptr<hpx::threads::policies::local_priority_queue_scheduler<>>
                                      scheduler(new hpx::threads::policies::local_priority_queue_scheduler<>(num_threads));

                                  auto mode = hpx::threads::policies::scheduler_mode(
                                      // hpx::threads::policies::do_background_work |
                                      hpx::threads::policies::reduce_thread_priority |
                                  // hpx::threads::policies::delay_exit |
                                      hpx::threads::policies::enable_elasticity);

                                  std::unique_ptr<hpx::threads::detail::thread_pool_base> pool(
                                      new hpx::threads::detail::scheduled_thread_pool<hpx::threads::policies::local_priority_queue_scheduler<>>(
                                          std::move(scheduler), notifier, pool_index, pool_name, mode,
                                          thread_offset));

                                  return pool;
                              });

        std::cout << "hpx::start(f, argc, argv);\n";
        hpx::util::high_resolution_timer t_first;
        hpx::start(argc, argv);
        double resume_time_first = t_first.elapsed();
        std::cout << "runtime started\n";
        hpx::stop(hpx::runtime_exit_mode_shutdown);
        double suspend_time_first = t_first.elapsed();
        std::cout << "runtime suspended\n";

        std::cout << "resume (first): " << resume_time_first << " s\n";
        std::cout << "suspend (first): " << suspend_time_first << " s\n";

        for (std::size_t i = 0; i < 10; ++i)
        {
            hpx::util::high_resolution_timer t;

            hpx::start(argc, argv);
            double resume_time = t.elapsed();

            std::cout << "runtime started\n";

            hpx::stop(hpx::runtime_exit_mode_suspend);
            double suspend_time = t.elapsed();

            std::cout << "resume: " << resume_time << " s\n";
            std::cout << "suspend: " << suspend_time << " s\n";

            hpx::compat::this_thread::sleep_for(std::chrono::seconds(1));
        }

        hpx::start(argc, argv);
        // hpx::async([]() { hpx::finalize(); });
        hpx::stop();
    }

    return hpx::util::report_errors();
}
