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
// - [ ] documentation
// - [ ] exception handling
// - [ ] allow yielding after some time?
// - [ ] custom executor parameters with smaller chunk size
// - [ ] make copy constructible (for policy_allocator)?
// - [ ] steal work
// - [ ] relax memory orderings
// - [ ] allow non-void functions

namespace hpx { namespace parallel { namespace execution { namespace experimental {
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
        class shared_data {
            threads::thread_pool_base* pool_ = nullptr;
            threads::thread_priority priority_ = threads::thread_priority_default;
            threads::thread_stacksize stacksize_ =
                threads::thread_stacksize_default;
            threads::thread_schedule_hint schedulehint_{};

            // Fixed data for the duration of the executor.
            std::size_t main_thread_;
            std::size_t num_threads_;
            std::vector<hpx::util::cache_aligned_data<std::atomic<bool>>>
                threads_active_;
            std::atomic<bool> stop_;

            // Changing data for each parallel region.
            using queue_type =
                hpx::concurrency::contiguous_index_queue<std::uint32_t>;
            using queues_type =
                std::vector<hpx::util::cache_aligned_data<queue_type>>;
            using function_wrapper_type = void(void*, void const*,
                void*, std::size_t, std::size_t, queues_type&);

            // The helper function that dispatches to the actual element function.
            std::atomic<function_wrapper_type*> function_wrapper_{nullptr};

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
                std::atomic<bool>& thread_active_;
                std::atomic<bool>& stop_;

                // Changing data for each parallel region.
                std::atomic<function_wrapper_type*>& function_wrapper_;
                std::atomic<void*>& element_function_;
                std::atomic<void const*>& shape_;
                std::atomic<std::size_t>& size_;
                std::atomic<void*>& argument_pack_;
                queues_type& queues_;

                void operator()()
                {
                    // Signal that thread started.
                    thread_active_ = false;

                    //printf("%zu has started\n", thread_index_);

                    // Wait for more work.
                    while (!thread_active_.load())
                    {
                    }

                    while (!stop_.load())
                    {
                        //printf("%zu running parallel region\n", thread_index_);

                        // Initialize local queue.
                        queue_type& local_queue = queues_[thread_index_].data_;

                        std::size_t const part_begin =
                            (thread_index_ * size_) / num_threads_;
                        std::size_t const part_end =
                            ((thread_index_ + 1) * size_) / num_threads_;
                        local_queue.reset(part_begin, part_end);

                        // Do computation.
                        //printf("%zu calling wrapper function\n", thread_index_);
                        (function_wrapper_.load())(element_function_.load(),
                            shape_.load(), argument_pack_.load(), thread_index_,
                            num_threads_, queues_);
                        //printf(
                            //"%zu done calling wrapper function\n", thread_index_);

                        // Done with this parallel region.
                        thread_active_ = false;

                        // Wait for more work.
                        while (!thread_active_.load())
                        {
                        }
                    }

                    //printf("%zu exiting work loop\n", thread_index_);

                    // Executor is destructed, no more work.
                    thread_active_ = false;
                }
            };

            void init_threads()
            {
                main_thread_ = get_local_worker_thread_num();
                num_threads_ = pool_->get_os_thread_count();
                stop_ = false;
                queues_.resize(num_threads_);

                for (std::size_t t = 0; t < num_threads_; ++t)
                {
                    if (t == main_thread_)
                    {
                        threads_active_[t].data_ = false;
                        continue;
                    }
                    threads_active_[t].data_ = true;
                    threads::thread_schedule_hint hint{
                        static_cast<std::int16_t>(t)};
                    hpx::detail::async_launch_policy_dispatch<
                        launch::async_policy>::call(launch::async, pool_,
                        threads::thread_priority_high, stacksize_, hint,
                        thread_function{num_threads_, t, threads_active_[t].data_,
                            stop_, function_wrapper_, element_function_, shape_,
                            size_, argument_pack_, queues_});
                }

                //printf("main thread is %zu\n", main_thread_);
                //printf("waiting for threads to start\n");

                // Wait for all threads to start.
                for (std::size_t t = 0; t < num_threads_; ++t)
                {
                    while (threads_active_[t].data_.load())
                    {
                    }
                    //printf("%zu started\n", t);
                }
                //printf("all threads started\n");
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
              , threads_active_(num_threads_)
            {
                HPX_ASSERT(pool_);
                init_threads();
            }

            ~shared_data()
            {
                stop_ = true;

                for (std::size_t t = 0; t < num_threads_; ++t)
                {
                    if (t == main_thread_)
                    {
                        threads_active_[t].data_ = false;
                        continue;
                    }
                    threads_active_[t].data_ = true;
                }

                // Wait for threads to terminate.
                for (std::size_t t = 0; t < num_threads_; ++t)
                {
                    while (threads_active_[t].data_)
                    {
                    }
                }
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
            struct function_wrapper_helper
            {
                using function_type = typename std::decay<F>::type;
                using shape_type = typename std::decay<S>::type;
                using argument_pack_type = typename std::decay<Tuple>::type;
                using index_pack_type = typename hpx::util::detail::fused_index_pack<Tuple>::type;

                template <std::size_t... Is_, typename F_, typename A_, typename Tuple_>
                static void invoke_helper(hpx::util::index_pack<Is_...>, F_&& f, A_&& a, Tuple_&& t) {
                    hpx::util::invoke(f, a, hpx::util::get<Is_>(std::forward<Tuple_>(t))...);
                }

                static void invoke(void* element_function_void,
                    void const* shape_void, void* argument_pack_void,
                    std::size_t thread_index, std::size_t num_threads,
                    queues_type& queues)
                {
                    F& element_function = *static_cast<F*>(element_function_void);
                    S const& shape = *static_cast<S const*>(shape_void);
                    Tuple argument_pack =
                        *static_cast<Tuple*>(argument_pack_void);

                    queue_type& local_queue = queues[thread_index].data_;

                    hpx::util::optional<std::uint32_t> index;

                    // Process local items first.
                    while (index = local_queue.pop_left())
                    {
                        //printf("%zu processing index %u\n", thread_index,
                            //index.value());
                        auto it = hpx::util::begin(shape);
                        for (std::size_t i = 0; i < index.value(); ++i, ++it);
                        invoke_helper(index_pack_type{}, element_function,
                            *it, argument_pack);
                    }

                    //printf("%zu done processing local queue\n", thread_index);
                };
            };

        public:
            template <typename F, typename S, typename... Ts>
            void bulk_sync_execute(F&& f, S const& shape, Ts&&... ts)
            {
                //printf("bulk_sync_execute\n");

                // Wait for all threads to be idle.
                for (std::size_t t = 0; t < num_threads_; ++t)
                {
                    while (threads_active_[t].data_)
                    {
                    }
                }

                //printf("all threads idle\n");

                // Set the function.
                element_function_ = static_cast<void*>(&f);
                shape_ = static_cast<void const*>(&shape);
                size_ = hpx::util::size(shape);
                auto argument_pack =
                    hpx::util::make_tuple<>(std::forward<Ts>(ts)...);
                argument_pack_ = static_cast<void*>(&argument_pack);
                function_wrapper_ = static_cast<function_wrapper_type*>(
                    &function_wrapper_helper<typename std::decay<F>::type,
                        typename std::decay<
                            S>::type, decltype(argument_pack)>::invoke);

                // Signal threads to calculate their local work.
                std::size_t thread_index_ = main_thread_;
                queue_type& local_queue = queues_[thread_index_].data_;
                std::size_t const part_begin =
                    (thread_index_ * size_) / num_threads_;
                std::size_t const part_end =
                    ((thread_index_ + 1) * size_) / num_threads_;
                local_queue.reset(part_begin, part_end);

                //printf("signal threads to start\n");

                // Signal threads to start working.
                for (std::size_t t = 0; t < num_threads_; ++t)
                {
                    threads_active_[t].data_ = true;
                }

                hpx::util::optional<std::uint32_t> index;

                while (index = local_queue.pop_left())
                {
                    auto it = hpx::util::begin(shape);
                    for (std::size_t i = 0; i < index.value(); ++i, ++it);

                    //printf(
                        //"%zu processing index %u\n", thread_index_, index.value());
                    hpx::util::invoke(f, *it, ts...);
                }

                //printf("%zu done processing local queue\n", thread_index_);

                threads_active_[main_thread_].data_ = false;

                //printf("wait for threads to be idle\n");

                // Wait for threads to finish.
                for (std::size_t t = 0; t < num_threads_; ++t)
                {
                    //printf("%zu idle\n", t);
                    while (threads_active_[t].data_)
                    {
                    }
                }

                //printf("all threads idle\n");
            }

            template <typename F, typename S, typename... Ts>
            std::vector<hpx::future<typename detail::bulk_function_result<F, S, Ts...>::type>> bulk_async_execute(F&& f, S const& shape, Ts&&... ts)
            {
                bulk_sync_execute(std::forward<F>(f), shape, std::forward<Ts>(ts)...);
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
            shared_->bulk_sync_execute(std::forward<F>(f), shape, std::forward<Ts>(ts)...);
        }

        template <typename F, typename S, typename... Ts>
        decltype(auto) bulk_async_execute(F&& f, S const& shape, Ts&&... ts)
        {
            return shared_->bulk_async_execute(std::forward<F>(f), shape, std::forward<Ts>(ts)...);
        }
    };
}}}}   // namespace hpx::parallel::execution

namespace hpx { namespace parallel { namespace execution {
    /// \cond NOINTERNAL
    template <>
    struct is_one_way_executor<parallel::execution::experimental::fork_join_executor>
      : std::true_type
    {
    };

    template <>
    struct is_two_way_executor<parallel::execution::experimental::fork_join_executor>
      : std::true_type
    {
    };

    template <>
    struct is_bulk_one_way_executor<parallel::execution::experimental::fork_join_executor>
      : std::true_type
    {
    };

    template <>
    struct is_bulk_two_way_executor<parallel::execution::experimental::fork_join_executor>
      : std::true_type
    {
    };
    /// \endcond
}}}    // namespace hpx::parallel::execution
