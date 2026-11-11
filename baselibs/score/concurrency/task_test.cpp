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
#include "score/concurrency/task.h"

#include "score/concurrency/executor.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

namespace score
{
namespace concurrency
{
namespace
{

class TestTask : public Task
{
  public:
    ~TestTask() override = default;

    MOCK_METHOD(void, Operator, (const score::cpp::stop_token), (noexcept));

    void operator()(const score::cpp::stop_token token) noexcept override
    {
        Operator(token);
    }

    MOCK_METHOD(score::cpp::stop_source, GetStopSource, (), (const, noexcept, override));
};

TEST(TaskTest, ConstructionAndDestructionOnStack)
{
    TestTask task{};
}

TEST(TaskTest, ConstructionAndDestructionOnHeap)
{
    auto unique_task = std::make_unique<TestTask>();
    unique_task.reset();
}

TEST(TaskTest, ConstructionAndDestructionOnHeapWithBasePointer)
{
    std::unique_ptr<Task> unique_task = std::make_unique<TestTask>();
    unique_task.reset();
}

}  // namespace
}  // namespace concurrency
}  // namespace score
