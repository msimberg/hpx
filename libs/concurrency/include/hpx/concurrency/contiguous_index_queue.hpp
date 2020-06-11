//  Copyright (c) 2020 Mikael Simberg
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <hpx/modules/assertion.hpp>
#include <hpx/concurrency/cache_line_data.hpp>
#include <hpx/modules/datastructures.hpp>

#include <atomic>
#include <cstdint>

namespace hpx { namespace concurrency {
    template <typename T = std::uint32_t>
    class contiguous_index_queue
    {
        static_assert(sizeof(T) <= 4,
            "contiguous_index_queue assumes at most 32 bit indices to fit two "
            "indices in an at most 64 bit struct");
        static_assert(std::is_integral<T>::value,
            "contiguous_index_queue only works with integral indices");

        struct range
        {
            T first = 0;
            T last = 0;

            range() = default;

            range(T first, T last)
              : first(first)
              , last(last)
            {
            }

            constexpr range increment_first()
            {
                return range{first + 1, last};
            }

            constexpr range decrement_last()
            {
                return range{first, last - 1};
            }

            constexpr bool empty()
            {
                return first >= last;
            }
        };

    public:
        constexpr void reset(T first, T last) noexcept
        {
            initial_range = {first, last};
            current_range.data_ = {first, last};
            HPX_ASSERT(first <= last);
        }

        constexpr contiguous_index_queue() noexcept
          : initial_range{}
          , current_range{}
        {
        }

        constexpr contiguous_index_queue(T first, T last) noexcept
          : initial_range{}
          , current_range{}
        {
            reset(first, last);
        }

        constexpr contiguous_index_queue(contiguous_index_queue<T>&& other)
          : initial_range{other.initial_range}
          , current_range{}
        {
            current_range.data_ = other.current_range.data_.load();
        }

        // TODO: Move assignment, copy constructor, copy assignment

        constexpr hpx::util::optional<T> pop_left() noexcept
        {
            range expected_range{0, 0};
            range desired_range{0, 0};
            T index = 0;

            do
            {
                expected_range =
                    current_range.data_.load(std::memory_order_relaxed);

                if (expected_range.empty())
                {
                    return hpx::util::nullopt;
                }

                index = expected_range.first;
                desired_range = expected_range.increment_first();
            } while (!current_range.data_.compare_exchange_weak(
                expected_range, desired_range));

            return hpx::util::make_optional<>(index);
        }

        constexpr hpx::util::optional<T> pop_right() noexcept
        {
            range expected_range{0, 0};
            range desired_range{0, 0};
            T index = 0;

            do
            {
                expected_range =
                    current_range.data_.load(std::memory_order_relaxed);

                if (expected_range.empty())
                {
                    return hpx::util::nullopt;
                }

                desired_range = expected_range.decrement_last();
                index = desired_range.last;
            } while (!current_range.data_.compare_exchange_weak(
                expected_range, desired_range));

            return hpx::util::make_optional(index);
        }

    private:
        range initial_range;
        hpx::util::cache_line_data<std::atomic<range>> current_range;
    };
}}    // namespace hpx::concurrency

// t1     t2
// 0
// 1      3
//        2
// 1
// 2
//
// 2      2
// 3      1
