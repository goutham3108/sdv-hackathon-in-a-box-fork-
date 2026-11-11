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
#include "score/concurrency/type_traits.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <mutex>
#include <shared_mutex>

using namespace score;

namespace
{

class HasLockOnly
{
  public:
    void lock() {}
};

class HasUnlockOnly
{
  public:
    void unlock() {}
};

class HasLockAndUnlockMismatchedSignature
{
  public:
    void lock(int&) {}
    void unlock(int*) {}
};

class BasicLockableArchetype
{
  public:
    void lock() {}
    void unlock() {}
};

}  // namespace

TEST(TypeTraitsTest, IsBasicLockableCompileTimeChecks)
{
    // Negative cases
    EXPECT_TRUE((!is_basic_lockable_v<HasLockOnly>)) << "HasLockOnly should not be basic_lockable";
    EXPECT_TRUE((!is_basic_lockable_v<HasUnlockOnly>)) << "HasUnlockOnly should not be basic_lockable";
    EXPECT_TRUE((!is_basic_lockable_v<HasLockAndUnlockMismatchedSignature>))
        << "HasLockAndUnlockMismatchedSignature should not be basic_lockable";

    // Positive cases
    EXPECT_TRUE((is_basic_lockable_v<BasicLockableArchetype>)) << "BasicLockableArchetype should be basic_lockable";

    // Standard library types
    EXPECT_TRUE((is_basic_lockable_v<std::mutex>)) << "std::mutex should be basic_lockable";
    EXPECT_TRUE((is_basic_lockable_v<std::timed_mutex>)) << "std::timed_mutex should be basic_lockable";
    EXPECT_TRUE((is_basic_lockable_v<std::recursive_mutex>)) << "std::recursive_mutex should be basic_lockable";
    EXPECT_TRUE((is_basic_lockable_v<std::recursive_timed_mutex>))
        << "std::recursive_timed_mutex should be basic_lockable";

    EXPECT_TRUE((is_basic_lockable_v<std::shared_mutex>)) << "std::shared_mutex should be basic_lockable";
    EXPECT_TRUE((is_basic_lockable_v<std::shared_timed_mutex>)) << "std::shared_timed_mutex should be basic_lockable";

    EXPECT_TRUE((is_basic_lockable_v<std::unique_lock<std::mutex>>))
        << "std::unique_lock<std::mutex> should be basic_lockable";
    EXPECT_TRUE((is_basic_lockable_v<std::shared_lock<std::shared_mutex>>))
        << "std::shared_lock<std::shared_mutex> should be basic_lockable";
}
