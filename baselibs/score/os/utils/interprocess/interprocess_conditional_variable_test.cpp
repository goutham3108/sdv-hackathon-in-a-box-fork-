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
#include "score/os/utils/interprocess/interprocess_conditional_variable.h"

#include "score/stop_token.hpp"

#include "gtest/gtest.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

namespace score
{
namespace os
{
namespace
{

TEST(InterprocessConditionalVariable, WaitsAndNotifiesAll)
{
    // Given multiple threads that wait with a predicate on a conditional variable
    constexpr auto numberOfThreads{5};
    using namespace std::chrono_literals;
    InterprocessMutex mutex{};
    InterprocessConditionalVariable unit{};
    bool flag{false};
    std::atomic<bool> executed{false};

    std::vector<std::thread> threads{};
    for (std::int32_t counter{0}; counter < numberOfThreads; counter++)
    {
        threads.emplace_back([&unit, &mutex, &flag, &executed]() {
            std::unique_lock<InterprocessMutex> lock{mutex};
            unit.wait(lock, [&flag]() {
                return flag;
            });
            executed = true;
        });
    }

    std::this_thread::sleep_for(10ms);
    ASSERT_FALSE(executed);

    // When changing the predicate and notifying all of them
    {
        std::lock_guard<InterprocessMutex> lock{mutex};
        flag = true;
    }
    unit.notify_all();

    // That all of them get woken up
    for (auto& thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

TEST(InterprocessConditionalVariable, WaitsAndNotifiesOne)
{
    // Given one thread that waits on a predicate
    using namespace std::chrono_literals;
    InterprocessMutex mutex{};
    InterprocessConditionalVariable unit{};
    bool flag{false};
    std::atomic<bool> executed{false};

    std::thread t1{[&unit, &mutex, &flag, &executed]() {
        std::unique_lock<InterprocessMutex> lock{mutex};
        unit.wait(lock, [&flag]() {
            return flag;
        });
        executed = true;
    }};

    std::this_thread::sleep_for(10ms);
    ASSERT_FALSE(executed);

    // When changing the predicate and notifying one thread
    {
        std::lock_guard<InterprocessMutex> lock{mutex};
        flag = true;
    }
    unit.notify_one();

    // That this thread gets woken up
    t1.join();
}

TEST(InterprocessConditionalVariable, WaitFail)
{
    InterprocessConditionalVariable unit{};
    std::unique_lock<InterprocessMutex> empty_lock{};

    EXPECT_DEATH(unit.wait(empty_lock), ".*");
}

}  // namespace
}  // namespace os
}  // namespace score
