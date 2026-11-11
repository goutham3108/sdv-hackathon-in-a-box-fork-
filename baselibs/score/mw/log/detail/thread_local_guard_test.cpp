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
#include "score/mw/log/detail/thread_local_guard.h"

#include "score/concurrency/notification.h"

#include "gtest/gtest.h"

#include <thread>

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

TEST(ThreadLocalGuardTest, ByDefaultNotWithinLoggingStack)
{
    // Given that nothing has happened so far
    // When querying if we are in the logging stack
    // Then we should be not.
    EXPECT_FALSE(ThreadLocalGuard::IsWithingLogging());
}

TEST(ThreadLocalGuardTest, OnConstructionInLoggingStack)
{
    // Given the ThreadLocalGuard has been initialized
    ThreadLocalGuard unit{};

    // When querying if we are in the logging stack
    // Then we should be.
    EXPECT_TRUE(ThreadLocalGuard::IsWithingLogging());
}

TEST(ThreadLocalGuardTest, OnDenstructionNotInLoggingStack)
{
    // Given the ThreadLocalGuard has been constructed and destructed
    {
        ThreadLocalGuard unit{};
    }

    // When querying if we are in the logging stack
    // Then we should be not.
    EXPECT_FALSE(ThreadLocalGuard::IsWithingLogging());
}

TEST(ThreadLocalGuardTest, DifferentThreadDifferentResult)
{
    // Given in one thread the ThreadLocalGuard has been initialized
    ThreadLocalGuard unit{};

    concurrency::Notification notification{};
    std::thread other_thread{[&notification]() {
        // When checking in another thread
        // Then this is not affected
        EXPECT_FALSE(ThreadLocalGuard::IsWithingLogging());
        notification.notify();
    }};

    notification.waitWithAbort({});

    // When checking in the same thread
    // Then this is affected
    EXPECT_TRUE(ThreadLocalGuard::IsWithingLogging());

    other_thread.join();
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
