//  Copyright (c) 2020 ETH Zurich
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/modules/assertion.hpp>
#include <hpx/modules/async_base.hpp>
#include <hpx/modules/concurrency.hpp>
#include <hpx/modules/execution.hpp>
#include <hpx/modules/functional.hpp>
#include <hpx/modules/threading_base.hpp>
#include <hpx/modules/type_support.hpp>

#include <vector>

// TODO
// - [X] handle additional arguments
// - [X] make copy constructible (for policy_allocator)?
// - [X] add helpers for synchronization
// - [X] add states for synchronization
// - [ ] steal work
// - [ ] documentation
// - [ ] exception handling
// - [ ] allow yielding after some time?
// - [ ] custom executor parameters with smaller chunk size
// - [ ] relax memory orderings
// - [ ] allow non-void functions

namespace hpx { namespace parallel { namespace execution {
    namespace experimental {
        class fork_join_executor
        {
        public:
            /// Associate the parallel_execution_tag executor tag type as a default
            /// with this executor.
            typedef parallel_execution_tag execution_category;

            /// Associate the static_chunk_size executor parameters type as a default
            /// with this executor.
            typedef static_chunk_size executor_parameters_type;

        private:
            class shared_data
            {
                threads::thread_pool_base* pool_ = nullptr;
                threads::thread_priority priority_ =
                    threads::thread_priority_default;
                threads::thread_stacksize stacksize_ =
                    threads::thread_stacksize_default;
                threads::thread_schedule_hint schedulehint_{};

                enum class thread_state
                {
                    starting = 0,
                    idle = 1,
                    active = 2,
                    stopping = 3,
                    stopped = 4,
                };

                // Fixed data for the duration of the executor.
                std::size_t main_thread_;
                std::size_t num_threads_;
                std::vector<
                    hpx::util::cache_aligned_data<std::atomic<thread_state>>>
                    thread_states_;

                // Changing data for each parallel region.
                using queue_type =
                    hpx::concurrency::contiguous_index_queue<std::uint32_t>;
                using queues_type =
                    std::vector<hpx::util::cache_aligned_data<queue_type>>;
                using thread_function_helper_type = void(void*, void const*,
                    void*, std::size_t, std::size_t, queues_type&);

                // The helper function that dispatches to the actual element function.
                std::atomic<thread_function_helper_type*>
                    thread_function_helper_{nullptr};

                // User inputs.
                std::atomic<void*> element_function_{nullptr};
                std::atomic<void const*> shape_{nullptr};
                std::atomic<std::size_t> size_{0};
                std::atomic<void*> argument_pack_{nullptr};

                // The current queues for each worker HPX thread.
                queues_type queues_;

                // Entry point for each worker HPX thread. Holds references to the
                // member variables of fork_join_executor.
                struct thread_function
                {
                    // Fixed data for the duration of the executor.
                    std::size_t const num_threads_;
                    std::size_t const thread_index_;
                    std::atomic<thread_state>& thread_state_;

                    // Changing data for each parallel region.
                    std::atomic<thread_function_helper_type*>&
                        thread_function_helper_;
                    std::atomic<void*>& element_function_;
                    std::atomic<void const*>& shape_;
                    std::atomic<std::size_t>& size_;
                    std::atomic<void*>& argument_pack_;
                    queues_type& queues_;

                    void wait_nonidle_this_thread()
                    {
                        while (thread_state_.load() == thread_state::idle)
                        {
                        }
                    }

                    void set_idle_this_thread()
                    {
                        thread_state_ = thread_state::idle;
                    }

                    void set_stopped_this_thread()
                    {
                        thread_state_ = thread_state::stopped;
                    }

                    void operator()()
                    {
                        HPX_ASSERT(thread_state_ == thread_state::starting);
                        set_idle_this_thread();
                        wait_nonidle_this_thread();

                        while (thread_state_ < thread_state::stopping)
                        {
                            (thread_function_helper_.load())(
                                element_function_.load(), shape_.load(),
                                argument_pack_.load(), thread_index_,
                                num_threads_, queues_);

                            set_idle_this_thread();
                            wait_nonidle_this_thread();
                        }

                        HPX_ASSERT(thread_state_ == thread_state::stopping);
                        set_stopped_this_thread();
                    }
                };

                void set_idle_main_thread()
                {
                    thread_states_[main_thread_].data_ = thread_state::idle;
                }

                void set_stopped_main_thread()
                {
                    thread_states_[main_thread_].data_ = thread_state::stopped;
                }

                void set_stopping_all()
                {
                    for (std::size_t t = 0; t < num_threads_; ++t)
                    {
                        thread_states_[t].data_ = thread_state::stopping;
                    }
                }

                void set_active_all()
                {
                    for (std::size_t t = 0; t < num_threads_; ++t)
                    {
                        thread_states_[t].data_ = thread_state::active;
                    }
                }

                void wait_idle_all()
                {
                    for (std::size_t t = 0; t < num_threads_; ++t)
                    {
                        while (thread_states_[t].data_.load() !=
                            thread_state::idle)
                        {
                        }
                    }
                }

                void wait_stopped_all()
                {
                    for (std::size_t t = 0; t < num_threads_; ++t)
                    {
                        while (thread_states_[t].data_.load() !=
                            thread_state::stopped)
                        {
                        }
                    }
                }

                void init_threads()
                {
                    main_thread_ = get_local_worker_thread_num();
                    num_threads_ = pool_->get_os_thread_count();
                    queues_.resize(num_threads_);

                    for (std::size_t t = 0; t < num_threads_; ++t)
                    {
                        if (t == main_thread_)
                        {
                            thread_states_[t].data_ = thread_state::idle;
                            continue;
                        }

                        thread_states_[t].data_ = thread_state::starting;
                        threads::thread_schedule_hint hint{
                            static_cast<std::int16_t>(t)};
                        hpx::detail::async_launch_policy_dispatch<
                            launch::async_policy>::call(launch::async, pool_,
                            threads::thread_priority_high, stacksize_, hint,
                            thread_function{num_threads_, t,
                                thread_states_[t].data_,
                                thread_function_helper_, element_function_,
                                shape_, size_, argument_pack_, queues_});
                    }

                    wait_idle_all();
                }

                static void init_local_work_queue(queue_type& queue,
                    std::size_t thread_index, std::size_t num_threads,
                    std::size_t size)
                {
                    std::size_t const part_begin =
                        (thread_index * size) / num_threads;
                    std::size_t const part_end =
                        ((thread_index + 1) * size) / num_threads;
                    queue.reset(part_begin, part_end);
                }

            public:
                /// Create a new parallel executor
                explicit shared_data(threads::thread_priority priority =
                                         threads::thread_priority_default,
                    threads::thread_stacksize stacksize =
                        threads::thread_stacksize_default,
                    threads::thread_schedule_hint schedulehint = {})
                  : pool_(this_thread::get_pool())
                  , priority_(priority)
                  , stacksize_(stacksize)
                  , schedulehint_(schedulehint)
                  , num_threads_(pool_->get_os_thread_count())
                  , thread_states_(num_threads_)
                {
                    HPX_ASSERT(pool_);
                    init_threads();
                }

                ~shared_data()
                {
                    wait_idle_all();
                    set_stopping_all();
                    set_stopped_main_thread();
                    wait_stopped_all();
                }

                /// \cond NOINTERNAL
                bool operator==(shared_data const& rhs) const noexcept
                {
                    return pool_ == rhs.pool_ && priority_ == rhs.priority_ &&
                        stacksize_ == rhs.stacksize_ &&
                        schedulehint_ == rhs.schedulehint_;
                }

                bool operator!=(shared_data const& rhs) const noexcept
                {
                    return !(*this == rhs);
                }

                shared_data const& context() const noexcept
                {
                    return *this;
                }

            private:
                template <typename F, typename S, typename Tuple>
                struct thread_function_helper
                {
                    using function_type = typename std::decay<F>::type;
                    using shape_type = typename std::decay<S>::type;
                    using argument_pack_type = typename std::decay<Tuple>::type;
                    using index_pack_type =
                        typename hpx::util::detail::fused_index_pack<
                            Tuple>::type;

                    template <std::size_t... Is_, typename F_, typename A_,
                        typename Tuple_>
                    static void invoke_helper(hpx::util::index_pack<Is_...>,
                        F_&& f, A_&& a, Tuple_&& t)
                    {
                        hpx::util::invoke(f, a,
                            hpx::util::get<Is_>(std::forward<Tuple_>(t))...);
                    }

                    static void invoke(void* element_function_void,
                        void const* shape_void, void* argument_pack_void,
                        std::size_t thread_index, std::size_t num_threads,
                        queues_type& queues)
                    {
                        F& element_function =
                            *static_cast<F*>(element_function_void);
                        S const& shape = *static_cast<S const*>(shape_void);
                        Tuple argument_pack =
                            *static_cast<Tuple*>(argument_pack_void);

                        queue_type& local_queue = queues[thread_index].data_;
                        std::size_t size = hpx::util::size(shape);
                        init_local_work_queue(
                            local_queue, thread_index, num_threads, size);

                        hpx::util::optional<std::uint32_t> index;

                        // Process local items first.
                        while (index = local_queue.pop_left())
                        {
                            // TODO: Is there a better way to do this? Could
                            // define the required operations on the iterator.
                            auto it = hpx::util::begin(shape);
                            for (std::size_t i = 0; i < index.value();
                                 ++i, ++it)
                                ;
                            invoke_helper(index_pack_type{}, element_function,
                                *it, argument_pack);
                        }
                    };
                };

            public:
                template <typename F, typename S, typename... Ts>
                void bulk_sync_execute(F&& f, S const& shape, Ts&&... ts)
                {
                    wait_idle_all();

                    // Set the function.
                    element_function_ = static_cast<void*>(&f);
                    shape_ = static_cast<void const*>(&shape);
                    size_ = hpx::util::size(shape);
                    auto argument_pack =
                        hpx::util::make_tuple<>(std::forward<Ts>(ts)...);
                    argument_pack_ = static_cast<void*>(&argument_pack);
                    thread_function_helper_ = static_cast<
                        thread_function_helper_type*>(
                        &thread_function_helper<typename std::decay<F>::type,
                            typename std::decay<S>::type,
                            decltype(argument_pack)>::invoke);

                    set_active_all();

                    std::size_t thread_index_ = main_thread_;
                    queue_type& local_queue = queues_[thread_index_].data_;
                    init_local_work_queue(
                        local_queue, main_thread_, num_threads_, size_);

                    // Main work loop
                    hpx::util::optional<std::uint32_t> index;

                    while (index = local_queue.pop_left())
                    {
                        // TODO: Is there a better way to do this? Could define
                        // the required operations on the iterator.
                        auto it = hpx::util::begin(shape);
                        for (std::size_t i = 0; i < index.value(); ++i, ++it)
                            ;
                        hpx::util::invoke(f, *it, ts...);
                    }

                    set_idle_main_thread();
                    wait_idle_all();
                }

                template <typename F, typename S, typename... Ts>
                std::vector<hpx::future<
                    typename detail::bulk_function_result<F, S, Ts...>::type>>
                bulk_async_execute(F&& f, S const& shape, Ts&&... ts)
                {
                    bulk_sync_execute(
                        std::forward<F>(f), shape, std::forward<Ts>(ts)...);
                    return {};
                }
                /// \endcond
            };

        private:
            std::shared_ptr<shared_data> shared_;

        public:
            explicit fork_join_executor(threads::thread_priority priority =
                                            threads::thread_priority_default,
                threads::thread_stacksize stacksize =
                    threads::thread_stacksize_default,
                threads::thread_schedule_hint schedulehint = {})
              : shared_(new shared_data(priority, stacksize, schedulehint))
            {
            }

            template <typename F, typename S, typename... Ts>
            void bulk_sync_execute(F&& f, S const& shape, Ts&&... ts)
            {
                shared_->bulk_sync_execute(
                    std::forward<F>(f), shape, std::forward<Ts>(ts)...);
            }

            template <typename F, typename S, typename... Ts>
            decltype(auto) bulk_async_execute(F&& f, S const& shape, Ts&&... ts)
            {
                return shared_->bulk_async_execute(
                    std::forward<F>(f), shape, std::forward<Ts>(ts)...);
            }
        };
}}}}    // namespace hpx::parallel::execution::experimental

namespace hpx { namespace parallel { namespace execution {
    /// \cond NOINTERNAL
    template <>
    struct is_one_way_executor<
        parallel::execution::experimental::fork_join_executor> : std::true_type
    {
    };

    template <>
    struct is_two_way_executor<
        parallel::execution::experimental::fork_join_executor> : std::true_type
    {
    };

    template <>
    struct is_bulk_one_way_executor<
        parallel::execution::experimental::fork_join_executor> : std::true_type
    {
    };

    template <>
    struct is_bulk_two_way_executor<
        parallel::execution::experimental::fork_join_executor> : std::true_type
    {
    };
    /// \endcond
}}}    // namespace hpx::parallel::execution
