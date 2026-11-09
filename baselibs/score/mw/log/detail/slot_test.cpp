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
#include "score/mw/log/detail/slot.h"

#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <thread>
#include <vector>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{

////////////////////////////////////////////////////////////////////////////////
// Test Fixture for Slot
////////////////////////////////////////////////////////////////////////////////
class SlotFixture : public ::testing::Test
{
  protected:
    // You can put common setup/teardown logic here if needed.
    // For simple classes like Slot, it’s often empty.
};

TEST_F(SlotFixture, DefaultConstructor_InitialState)
{
    // Given a Slot with default constructor
    Slot<int> slot;

    // Then it should not be in use
    EXPECT_FALSE(slot.IsUsed());

    // And the data should be value-initialized (for int, that is 0)
    EXPECT_EQ(slot.GetDataRef(), 0);
}

TEST_F(SlotFixture, ConstructorWithValue_InitialState)
{
    // Given a Slot constructed with an initial value
    Slot<int> slot(123);

    // Then it should not be in use
    EXPECT_FALSE(slot.IsUsed());

    // And the data should match the constructor's value
    EXPECT_EQ(slot.GetDataRef(), 123);
}

TEST_F(SlotFixture, TryUseSetsSlotInUse)
{
    // Given a default-constructed Slot
    Slot<int> slot;
    EXPECT_FALSE(slot.IsUsed());

    // When we call TryUse()
    bool first_attempt = slot.TryUse();

    // Then it should succeed and the slot is now in use
    EXPECT_TRUE(first_attempt);
    EXPECT_TRUE(slot.IsUsed());

    // If we try again, it should fail because it's already in use
    bool second_attempt = slot.TryUse();
    EXPECT_FALSE(second_attempt);
    EXPECT_TRUE(slot.IsUsed());
}

TEST_F(SlotFixture, ReleaseMakesSlotReusable)
{
    // Given a Slot that is already in use
    Slot<int> slot;
    EXPECT_TRUE(slot.TryUse());
    EXPECT_TRUE(slot.IsUsed());

    // When we release the slot
    slot.Release();

    // Then it should no longer be in use
    EXPECT_FALSE(slot.IsUsed());

    // And if we call TryUse again, it should succeed
    EXPECT_TRUE(slot.TryUse());
    EXPECT_TRUE(slot.IsUsed());
}

TEST_F(SlotFixture, GetDataRefReadWrite)
{
    // Given a default-constructed slot
    Slot<int> slot;
    EXPECT_EQ(slot.GetDataRef(), 0);

    // When we modify its data
    slot.SetData(999);

    // Then the data should reflect that change
    EXPECT_EQ(slot.GetDataRef(), 999);
}

TEST_F(SlotFixture, ConcurrentTryUseStressTest)
{
    constexpr std::size_t kThreadCounter = 8UL;
    Slot<int> slot;  // not in use at the start

    std::array<std::thread, kThreadCounter> threads;
    std::array<bool, kThreadCounter> results{};
    results.fill(false);

    // Launch multiple threads that attempt to claim the slot
    for (std::size_t i = 0UL; i < kThreadCounter; ++i)
    {
        threads[i] = std::thread([&slot, &results, i]() noexcept {
            // Each thread tries exactly once to claim the slot
            results[i] = slot.TryUse();
        });
    }

    // Join the threads
    for (auto& t : threads)
    {
        t.join();
    }

    // Exactly one thread should have successfully claimed the slot
    auto successes = std::count(results.begin(), results.end(), true);
    EXPECT_EQ(successes, 1U);

    // And slot should be in use
    EXPECT_TRUE(slot.IsUsed());
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
