//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/condition_variable.hpp>
#include <hpx/execution.hpp>
#include <hpx/functional.hpp>
#include <hpx/init.hpp>
#include <hpx/modules/testing.hpp>
#include <hpx/thread.hpp>

#include <array>
#include <atomic>
#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
void test_execute()
{
    hpx::thread::id parent_id = hpx::this_thread::get_id();

    hpx::execution::experimental::executor exec{};
    hpx::execution::experimental::execute(exec,
        [parent_id]() { HPX_TEST_NEQ(hpx::this_thread::get_id(), parent_id); });
}

struct check_context_receiver
{
    hpx::thread::id parent_id;
    hpx::lcos::local::condition_variable& cond;
    std::atomic<bool>& executed;

    template <typename E>
    void set_error(E&&) noexcept
    {
        HPX_TEST(false);
    }

    void set_done() noexcept
    {
        HPX_TEST(false);
    };

    template <typename... Ts>
    void set_value(Ts&&...) noexcept
    {
        HPX_TEST_NEQ(parent_id, hpx::this_thread::get_id());
        HPX_TEST_NEQ(hpx::thread::id(hpx::threads::invalid_thread_id),
            hpx::this_thread::get_id());
        executed = true;
        cond.notify_one();
    }
};

void test_sender_receiver_basic()
{
    hpx::thread::id parent_id = hpx::this_thread::get_id();
    hpx::lcos::local::mutex mtx;
    hpx::lcos::local::condition_variable cond;
    std::atomic<bool> executed{false};

    hpx::execution::experimental::executor exec{};

    auto begin = hpx::execution::experimental::schedule(exec);
    auto work = hpx::execution::experimental::connect(
        std::move(begin), check_context_receiver{parent_id, cond, executed});
    hpx::execution::experimental::start(std::move(work));

    {
        std::unique_lock<hpx::lcos::local::mutex> l{mtx};
        cond.wait(l, [&]() { return executed.load(); });
    }

    HPX_TEST(executed);
}

void test_sender_receiver_basic2()
{
    hpx::thread::id parent_id = hpx::this_thread::get_id();
    hpx::lcos::local::mutex mtx;
    hpx::lcos::local::condition_variable cond;
    std::atomic<bool> executed{false};

    hpx::execution::experimental::start(hpx::execution::experimental::connect(
        hpx::execution::experimental::executor{},
        check_context_receiver{parent_id, cond, executed}));

    {
        std::unique_lock<hpx::lcos::local::mutex> l{mtx};
        cond.wait(l, [&]() { return executed.load(); });
    }

    HPX_TEST(executed);
}

hpx::thread::id sender_receiver_transform_thread_id;

void test_sender_receiver_transform()
{
    hpx::execution::experimental::executor exec{};
    hpx::thread::id parent_id = hpx::this_thread::get_id();
    hpx::lcos::local::mutex mtx;
    hpx::lcos::local::condition_variable cond;
    std::atomic<bool> executed{false};

    auto begin = hpx::execution::experimental::schedule(exec);
    auto work1 =
        hpx::execution::experimental::transform(std::move(begin), [=]() {
            sender_receiver_transform_thread_id = hpx::this_thread::get_id();
            HPX_TEST_NEQ(sender_receiver_transform_thread_id, parent_id);
        });
    auto work2 =
        hpx::execution::experimental::transform(std::move(work1), []() {
            HPX_TEST_EQ(sender_receiver_transform_thread_id,
                hpx::this_thread::get_id());
        });
    auto end = hpx::execution::experimental::connect(
        std::move(work2), check_context_receiver{parent_id, cond, executed});
    hpx::execution::experimental::start(std::move(end));

    {
        std::unique_lock<hpx::lcos::local::mutex> l{mtx};
        cond.wait(l, [&]() { return executed.load(); });
    }

    HPX_TEST(executed);
}

void test_sender_receiver_transform_wait()
{
    hpx::execution::experimental::executor exec{};
    hpx::thread::id parent_id = hpx::this_thread::get_id();
    std::atomic<std::size_t> transform_count{0};
    std::atomic<bool> executed{false};

    auto begin = hpx::execution::experimental::schedule(exec);
    auto work1 = hpx::execution::experimental::transform(
        std::move(begin), [&transform_count, parent_id]() {
            sender_receiver_transform_thread_id = hpx::this_thread::get_id();
            HPX_TEST_NEQ(sender_receiver_transform_thread_id, parent_id);
            ++transform_count;
        });
    auto work2 = hpx::execution::experimental::transform(
        std::move(work1), [&transform_count, &executed]() {
            HPX_TEST_EQ(sender_receiver_transform_thread_id,
                hpx::this_thread::get_id());
            ++transform_count;
            executed = true;
        });
    hpx::execution::experimental::sync_wait(std::move(work2));
    HPX_TEST_EQ(transform_count, std::size_t(2));
    HPX_TEST(executed);
}

void test_sender_receiver_transform_sync_wait()
{
    hpx::execution::experimental::executor exec{};
    hpx::thread::id parent_id = hpx::this_thread::get_id();
    std::atomic<std::size_t> transform_count{0};

    auto begin = hpx::execution::experimental::schedule(exec);
    auto work = hpx::execution::experimental::transform(
        std::move(begin), [&transform_count, parent_id]() {
            sender_receiver_transform_thread_id = hpx::this_thread::get_id();
            HPX_TEST_NEQ(sender_receiver_transform_thread_id, parent_id);
            ++transform_count;
            return 42;
        });
    auto result = hpx::execution::experimental::sync_wait(std::move(work));
    HPX_TEST_EQ(transform_count, std::size_t(1));
    static_assert(
        std::is_same<int, typename std::decay<decltype(result)>::type>::value,
        "result should be an int");
    HPX_TEST_EQ(result, 42);
}

void test_sender_receiver_transform_arguments()
{
    hpx::execution::experimental::executor exec{};
    hpx::thread::id parent_id = hpx::this_thread::get_id();
    std::atomic<std::size_t> transform_count{0};

    auto begin = hpx::execution::experimental::schedule(exec);
    auto work1 = hpx::execution::experimental::transform(
        std::move(begin), [&transform_count, parent_id]() {
            sender_receiver_transform_thread_id = hpx::this_thread::get_id();
            HPX_TEST_NEQ(sender_receiver_transform_thread_id, parent_id);
            ++transform_count;
            return 3;
        });
    auto work2 = hpx::execution::experimental::transform(
        std::move(work1), [&transform_count](int x) -> std::string {
            HPX_TEST_EQ(sender_receiver_transform_thread_id,
                hpx::this_thread::get_id());
            ++transform_count;
            return std::string("hello") + std::to_string(x);
        });
    auto work3 = hpx::execution::experimental::transform(
        std::move(work2), [&transform_count](std::string s) {
            HPX_TEST_EQ(sender_receiver_transform_thread_id,
                hpx::this_thread::get_id());
            ++transform_count;
            return 2 * s.size();
        });
    auto result = hpx::execution::experimental::sync_wait(std::move(work3));
    HPX_TEST_EQ(transform_count, std::size_t(3));
    static_assert(std::is_same<std::size_t,
                      typename std::decay<decltype(result)>::type>::value,
        "result should be a std::size_t");
    HPX_TEST_EQ(result, std::size_t(12));
}

template <typename F>
struct callback_receiver
{
    std::decay_t<F> f;
    hpx::lcos::local::condition_variable& cond;
    std::atomic<bool>& executed;

    template <typename E>
    void set_error(E&&) noexcept
    {
        HPX_TEST(false);
    }

    void set_done() noexcept
    {
        HPX_TEST(false);
    };

    template <typename... Ts>
    void set_value(Ts&&...) noexcept
    {
        HPX_INVOKE(f);
        executed = true;
        cond.notify_one();
    }
};

void test_properties()
{
    hpx::execution::experimental::executor exec{};
    hpx::lcos::local::mutex mtx;
    hpx::lcos::local::condition_variable cond;
    std::atomic<bool> executed{false};

    constexpr std::array<hpx::threads::thread_priority, 3> priorities{
        {hpx::threads::thread_priority::low,
            hpx::threads::thread_priority::normal,
            hpx::threads::thread_priority::high}};

    for (auto const prio : priorities)
    {
        auto exec_prop =
            hpx::execution::experimental::make_with_priority(exec, prio);
        HPX_TEST_EQ(
            hpx::execution::experimental::get_priority(exec_prop), prio);

        auto check = [prio]() {
            HPX_TEST_EQ(prio, hpx::this_thread::get_priority());
        };
        executed = false;
        hpx::execution::experimental::start(
            hpx::execution::experimental::connect(
                hpx::execution::experimental::schedule(exec_prop),
                callback_receiver<decltype(check)>{check, cond, executed}));
        {
            std::unique_lock<hpx::lcos::local::mutex> l{mtx};
            cond.wait(l, [&]() { return executed.load(); });
        }

        HPX_TEST(executed);
    }

    constexpr std::array<hpx::threads::thread_stacksize, 4> stacksizes{
        {hpx::threads::thread_stacksize::small_,
            hpx::threads::thread_stacksize::medium,
            hpx::threads::thread_stacksize::large,
            hpx::threads::thread_stacksize::huge}};

    for (auto const stacksize : stacksizes)
    {
        auto exec_prop =
            hpx::execution::experimental::make_with_stacksize(exec, stacksize);
        HPX_TEST_EQ(
            hpx::execution::experimental::get_stacksize(exec_prop), stacksize);

        auto check = [stacksize]() {
            HPX_TEST_EQ(stacksize,
                hpx::threads::get_thread_id_data(hpx::threads::get_self_id())
                    ->get_stack_size_enum());
        };
        executed = false;
        hpx::execution::experimental::start(
            hpx::execution::experimental::connect(
                hpx::execution::experimental::schedule(exec_prop),
                callback_receiver<decltype(check)>{check, cond, executed}));
        {
            std::unique_lock<hpx::lcos::local::mutex> l{mtx};
            cond.wait(l, [&]() { return executed.load(); });
        }

        HPX_TEST(executed);
    }

    constexpr std::array<hpx::threads::thread_schedule_hint, 4> hints{
        {hpx::threads::thread_schedule_hint{},
            hpx::threads::thread_schedule_hint{1},
            hpx::threads::thread_schedule_hint{
                hpx::threads::thread_schedule_hint_mode::thread, 2},
            hpx::threads::thread_schedule_hint{
                hpx::threads::thread_schedule_hint_mode::numa, 3}}};

    for (auto const hint : hints)
    {
        auto exec_prop =
            hpx::execution::experimental::make_with_hint(exec, hint);
        HPX_TEST(hpx::execution::experimental::get_hint(exec_prop) == hint);

        // A hint is not guaranteed to be respected, so we only check that the
        // executor holds the property.
    }
}

void test_on_basic()
{
    hpx::execution::experimental::executor exec{};
    hpx::thread::id parent_id = hpx::this_thread::get_id();
    hpx::thread::id current_id;

    auto begin = hpx::execution::experimental::schedule(exec);
    auto work1 =
        hpx::execution::experimental::transform(begin, [=, &current_id]() {
            current_id = hpx::this_thread::get_id();
            HPX_TEST_NEQ(current_id, parent_id);
        });
    auto work2 =
        hpx::execution::experimental::transform(work1, [=, &current_id]() {
            HPX_TEST_EQ(current_id, hpx::this_thread::get_id());
        });
    auto on1 = hpx::execution::experimental::on(work2, exec);
    auto work3 =
        hpx::execution::experimental::transform(on1, [=, &current_id]() {
            hpx::thread::id new_id = hpx::this_thread::get_id();
            HPX_TEST_NEQ(current_id, new_id);
            current_id = new_id;
            HPX_TEST_NEQ(current_id, parent_id);
        });
    auto work4 =
        hpx::execution::experimental::transform(work3, [=, &current_id]() {
            HPX_TEST_EQ(current_id, hpx::this_thread::get_id());
        });
    auto on2 = hpx::execution::experimental::on(work4, exec);
    auto work5 =
        hpx::execution::experimental::transform(on2, [=, &current_id]() {
            hpx::thread::id new_id = hpx::this_thread::get_id();
            HPX_TEST_NEQ(current_id, new_id);
            current_id = new_id;
            HPX_TEST_NEQ(current_id, parent_id);
        });

    hpx::execution::experimental::sync_wait(work5);
}

void test_on_arguments()
{
    hpx::execution::experimental::executor exec{};
    hpx::thread::id parent_id = hpx::this_thread::get_id();
    hpx::thread::id current_id;

    auto begin = hpx::execution::experimental::schedule(exec);
    auto work1 =
        hpx::execution::experimental::transform(begin, [=, &current_id]() {
            current_id = hpx::this_thread::get_id();
            HPX_TEST_NEQ(current_id, parent_id);
            return 3;
        });
    auto work2 =
        hpx::execution::experimental::transform(work1, [=, &current_id](int x) {
            HPX_TEST_EQ(current_id, hpx::this_thread::get_id());
            return x / 2.0;
        });
    auto on1 = hpx::execution::experimental::on(work2, exec);
    auto work3 = hpx::execution::experimental::transform(
        on1, [=, &current_id](double x) {
            hpx::thread::id new_id = hpx::this_thread::get_id();
            HPX_TEST_NEQ(current_id, new_id);
            current_id = new_id;
            HPX_TEST_NEQ(current_id, parent_id);
            return x / 2;
        });
    auto work4 =
        hpx::execution::experimental::transform(work3, [=, &current_id](int x) {
            HPX_TEST_EQ(current_id, hpx::this_thread::get_id());
            return "result: " + std::to_string(x);
        });
    auto on2 = hpx::execution::experimental::on(work4, exec);
    auto work5 = hpx::execution::experimental::transform(
        on2, [=, &current_id](std::string s) {
            hpx::thread::id new_id = hpx::this_thread::get_id();
            HPX_TEST_NEQ(current_id, new_id);
            current_id = new_id;
            HPX_TEST_NEQ(current_id, parent_id);
            return s + "!";
        });

    auto result = hpx::execution::experimental::sync_wait(work5);
    static_assert(std::is_same<std::string,
                      typename std::decay<decltype(result)>::type>::value,
        "result should be a std::string");
    HPX_TEST_EQ(result, std::string("result: 0!"));
}

void test_on()
{
    hpx::execution::experimental::executor exec{};
    hpx::thread::id parent_id = hpx::this_thread::get_id();
    hpx::thread::id current_id;

    auto begin = hpx::execution::experimental::schedule(exec);
    auto work1 = hpx::execution::experimental::transform(begin, []() {
        std::cerr << "work1: " << hpx::this_thread::get_id() << std::endl;
    });
    auto work2 = hpx::execution::experimental::transform(work1, []() {
        std::cerr << "work2: " << hpx::this_thread::get_id() << std::endl;
    });
    auto on1 = hpx::execution::experimental::on(work2, exec);
    auto work3 = hpx::execution::experimental::transform(on1, []() {
        std::cerr << "work3: " << hpx::this_thread::get_id() << std::endl;
    });
    auto work4 = hpx::execution::experimental::transform(work3, []() {
        std::cerr << "work4: " << hpx::this_thread::get_id() << std::endl;
    });

    hpx::execution::experimental::sync_wait(work4);
    hpx::execution::experimental::sync_wait(work4);
}

void test_fork()
{
    hpx::execution::experimental::executor exec{};

    auto begin = hpx::execution::experimental::schedule(exec);
    auto work1 = hpx::execution::experimental::transform(begin, []() {
        std::cerr << "work1: " << hpx::this_thread::get_id() << std::endl;
    });
    auto work2 = hpx::execution::experimental::transform(work1, []() {
        std::cerr << "work2: " << hpx::this_thread::get_id() << std::endl;
    });
    auto fork1 = hpx::execution::experimental::fork(work2, exec);

    auto work3a = hpx::execution::experimental::transform(fork1, []() {
        std::cerr << "work3a: " << hpx::this_thread::get_id() << std::endl;
    });
    auto work4a = hpx::execution::experimental::transform(work3a, []() {
        std::cerr << "work4a: " << hpx::this_thread::get_id() << std::endl;
    });
    hpx::execution::experimental::sync_wait(work4a);

    auto work3b = hpx::execution::experimental::transform(fork1, []() {
        std::cerr << "work3b: " << hpx::this_thread::get_id() << std::endl;
    });
    auto work4b = hpx::execution::experimental::transform(work3b, []() {
        std::cerr << "work4b: " << hpx::this_thread::get_id() << std::endl;
    });
    hpx::execution::experimental::sync_wait(work4b);
}

void test_fork_join()
{
    hpx::execution::experimental::executor exec{};

    auto begin = hpx::execution::experimental::schedule(exec);
    auto work1 = hpx::execution::experimental::transform(begin, []() {
        std::cerr << "work1: " << hpx::this_thread::get_id() << std::endl;
    });
    auto work2 = hpx::execution::experimental::transform(work1, []() {
        std::cerr << "work2: " << hpx::this_thread::get_id() << std::endl;
    });
    auto fork1 = hpx::execution::experimental::fork(work2, exec);

    auto work3a = hpx::execution::experimental::transform(fork1, []() {
        std::cerr << "work3a: " << hpx::this_thread::get_id() << std::endl;
    });
    auto work4a = hpx::execution::experimental::transform(work3a, []() {
        std::cerr << "work4a: " << hpx::this_thread::get_id() << std::endl;
    });

    auto work3b = hpx::execution::experimental::transform(fork1, []() {
        std::cerr << "work3b: " << hpx::this_thread::get_id() << std::endl;
    });

    auto work4b = hpx::execution::experimental::transform(work3b, []() {
        std::cerr << "work4b: " << hpx::this_thread::get_id() << std::endl;
    });

    auto when1 = hpx::execution::experimental::when_all(work4a, work4b);
    auto work5 = hpx::execution::experimental::transform(when1, []() {
        std::cerr << "work5: " << hpx::this_thread::get_id() << std::endl;
    });
    auto work6 = hpx::execution::experimental::transform(work5, []() {
        std::cerr << "work5: " << hpx::this_thread::get_id() << std::endl;
    });
    hpx::execution::experimental::sync_wait(work6);
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main()
{
    test_execute();
    test_sender_receiver_basic();
    test_sender_receiver_basic2();
    test_sender_receiver_transform();
    test_sender_receiver_transform_wait();
    test_sender_receiver_transform_sync_wait();
    test_sender_receiver_transform_arguments();
    test_properties();
    test_on_basic();
    test_on_arguments();
    test_on();
    test_fork();
    test_fork_join();

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    HPX_TEST_EQ_MSG(
        hpx::init(argc, argv), 0, "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
