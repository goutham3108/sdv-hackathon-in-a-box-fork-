/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_MW_LOG_DETAIL_CIRCULAR_ALLOCATOR_H
#define SCORE_MW_LOG_DETAIL_CIRCULAR_ALLOCATOR_H

#include "score/assert.hpp"
#include "score/optional.hpp"
#include "slot.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

namespace score
{
namespace mw
{
namespace log
{

namespace detail
{

/// \brief A Ring-Buffer that allows multiple producers to stream data in a lock-free manner
///
/// \details This implementation is right now specific to our first iteration of our logging implementation.
/// As it can be seen below there is no way for a consumer to acquire data. Further, we need to make this implementation
/// Shared-Memory ready. But, for our first iteration this is good enough :).
///
/// \tparam T Any type that shall be stored within the Ring-Buffer.
template <typename T>
class CircularAllocator final
{
  public:
    /// \brief Constructs a Ring-Buffer of capacity, without further acquiring memory during runtime.
    ///
    /// \param capacity The size of how many elements of T shall be stored within the Ring-Buffer
    explicit CircularAllocator(std::size_t capacity, const T& initial_value = T{})
        : claimed_sequence_{}, buffer_(capacity)
    {
        for (auto& slot : buffer_)
        {
            slot.SetData(initial_value);
        }
    }

    /// \brief Starts a Transaction for a producer to stream data into a slot
    ///
    /// \return The slot in which data can be written
    ///
    /// \post Slot is acquired and able to be written
    score::cpp::optional<std::size_t> AcquireSlotToWrite() noexcept
    {
        const auto max_number_of_loops = buffer_.size();
        std::uint32_t loop_limiter{0UL};
        std::size_t slot_index{};
        do
        {
            loop_limiter++;
            slot_index = claimed_sequence_.fetch_add(1UL);
            slot_index %= buffer_.capacity();

            if (loop_limiter > max_number_of_loops)
            {
                return {};
            }
        } while (!buffer_.at(slot_index).TryUse());

        return slot_index;
    }

    /// \brief Get a buffer for a specific slot to write data into it
    ///
    /// \param slot The slot where the underlying buffer shall be returned
    /// \return The buffer of T for the respective slot
    ///
    /// \pre Slot is acquired by AcquireSlotToWrite()
    T& GetUnderlyingBufferFor(std::size_t slot) noexcept
    {
        return buffer_.at(slot).GetDataRef();
    }

    /// \brief Stops the transaction of manipulating a specific slot
    /// \param slot The slot that is now no longer manipulated
    ///
    /// \pre slot was acquired by AcquireSlotToWrite() and data was written via GetUnderlyingBufferFor()
    /// \post Slot is marked as finished and could be overwritten by another call to AcquireSlotToWrite()
    void ReleaseSlot(std::size_t slot) noexcept
    {
        buffer_.at(slot).Release();
    }

    /// \brief Returns number of used elements
    ///
    /// \return The number of used elements
    std::size_t GetUsedCount() noexcept
    {
        // Static cast is allowed as count_if should not return negative value:
        return static_cast<std::size_t>(std::count_if(buffer_.begin(), buffer_.end(), [](auto&& i) {
            return i.IsUsed();
        }));
    }

  private:
    std::atomic<std::size_t> claimed_sequence_;

    // For the beginning this is still an std::vector with standard allocator. Once we refactor the IPC to DataRouter,
    // this data type will be directly placed in SharedMemory and a custom allocator will be added.
    std::vector<Slot<T>> buffer_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_CIRCULAR_ALLOCATOR_H
