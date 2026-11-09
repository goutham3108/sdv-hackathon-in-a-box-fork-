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
#include "score/concurrency/thread_load_tracking/thread_load_tracking.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace score
{
namespace concurrency
{
namespace
{

const std::chrono::steady_clock::time_point kStartingPoint = std::chrono::steady_clock::now();

TEST(TestThreadLoadTracking, NoWorkNoWaitShallReturnEmptyOptional)
{
    ThreadLoadTracking tracking{};
    const auto result = tracking.Calculate();
    EXPECT_FALSE(result.work_load_percent.has_value());
}

TEST(TestThreadLoadTracking, OnlyWorkShallReturnHundredPercent)
{
    auto now_function_mock = [i = 0]() mutable noexcept -> std::chrono::steady_clock::time_point {
        std::chrono::steady_clock::time_point time_points[] = {kStartingPoint,
                                                               kStartingPoint + std::chrono::milliseconds(10)};
        auto time_point = time_points[i];
        i++;
        return time_point;
    };
    ThreadLoadTracking tracking{std::move(now_function_mock)};

    tracking.StartWorking();

    const auto result = tracking.Calculate();
    EXPECT_DOUBLE_EQ(result.work_load_percent.value(), 100.0);
}

TEST(TestThreadLoadTracking, OnlyWaitShallReturnHundredPercent)
{
    auto now_function_mock = [i = 0]() mutable noexcept -> std::chrono::steady_clock::time_point {
        std::chrono::steady_clock::time_point time_points[] = {kStartingPoint,
                                                               kStartingPoint + std::chrono::milliseconds(10)};
        auto time_point = time_points[i];
        i++;
        return time_point;
    };
    ThreadLoadTracking tracking{std::move(now_function_mock)};

    tracking.StartWaiting();

    const auto result = tracking.Calculate();
    EXPECT_DOUBLE_EQ(result.work_load_percent.value(), 0.0);
}

TEST(TestThreadLoadTracking, HalfWaitHalfWorkShallReturnFiftyPercentWorkLoad)
{
    auto now_function_mock = [i = 0]() mutable noexcept -> std::chrono::steady_clock::time_point {
        std::chrono::steady_clock::time_point time_points[] = {kStartingPoint,
                                                               kStartingPoint + std::chrono::seconds(1),
                                                               kStartingPoint + std::chrono::seconds(1),
                                                               kStartingPoint + std::chrono::seconds(2)};
        auto time_point = time_points[i];
        i++;
        return time_point;
    };
    ThreadLoadTracking tracking{std::move(now_function_mock)};

    auto wait = tracking.StartWaiting();
    wait.End();
    tracking.StartWorking();

    const auto result = tracking.Calculate();
    EXPECT_DOUBLE_EQ(result.work_load_percent.value(), 50.0);
}

}  // namespace
}  // namespace concurrency
}  // namespace score
