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
#include "score/os/utils/interprocess/interprocess_notification.h"

#include "gtest/gtest.h"

#include "score/stop_token.hpp"

#include <chrono>
#include <future>
#include <thread>
#include <utility>

namespace score
{
namespace os
{
namespace
{

TEST(InterprocessNotification, WaitsForNotification)
{
    using namespace std::chrono_literals;

    InterprocessNotification notification{};
    std::promise<void> promise{};
    auto f = promise.get_future();

    std::thread waitingThread{[&notification, promise = std::move(promise)]() mutable {
        notification.waitWithAbort(score::cpp::stop_token{});
        promise.set_value();
    }};

    ASSERT_EQ(f.wait_for(50ms), std::future_status::timeout);  // To ensure that we are blocking in the thread.
    notification.notify();

    waitingThread.join();
}

TEST(InterprocessNotification, WaitsForNotificationAfterNotifying)
{
    using namespace std::chrono_literals;

    InterprocessNotification notification{};
    std::promise<void> promise{};
    auto f = promise.get_future();

    std::thread waitingThread{[&notification, promise = std::move(promise)]() mutable {
        notification.waitWithAbort(score::cpp::stop_token{});
        promise.set_value();
    }};

    ASSERT_EQ(f.wait_for(50ms), std::future_status::timeout);  // To ensure that we are blocking in the thread.
    notification.notify();
    notification.reset();

    std::promise<void> promise2{};
    auto f2 = promise2.get_future();

    std::thread waitingThread2{[&notification, promise = std::move(promise2)]() mutable {
        notification.waitWithAbort(score::cpp::stop_token{});
        promise.set_value();
    }};

    ASSERT_EQ(f2.wait_for(50ms), std::future_status::timeout);  // To ensure that we are blocking in the thread.
    notification.notify();

    waitingThread.join();
    waitingThread2.join();
}

}  // namespace
}  // namespace os
}  // namespace score
