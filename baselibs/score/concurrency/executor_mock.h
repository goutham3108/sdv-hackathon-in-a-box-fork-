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
#ifndef SCORE_LIB_CONCURRENCY_EXECUTOR_MOCK_H
#define SCORE_LIB_CONCURRENCY_EXECUTOR_MOCK_H

#include "score/concurrency/executor.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace concurrency
{
namespace testing
{

class ExecutorMock : public concurrency::Executor
{
  public:
    ExecutorMock()
    {
        ON_CALL(*this, Shutdown).WillByDefault([this]() {
            shutdown_state_ = true;
        });
        ON_CALL(*this, ShutdownRequested).WillByDefault([this]() {
            return shutdown_state_;
        });
    }

    MOCK_METHOD(void, Enqueue, (score::cpp::pmr::unique_ptr<score::concurrency::Task>), (override));
    MOCK_METHOD(std::size_t, MaxConcurrencyLevel, (), (const, noexcept, override));
    MOCK_METHOD(bool, ShutdownRequested, (), (const, noexcept, override));
    MOCK_METHOD(void, Shutdown, (), (noexcept, override));

  private:
    bool shutdown_state_{false};
};  // namespace testing

}  // namespace testing
}  // namespace concurrency
}  // namespace score
#endif  // SCORE_LIB_CONCURRENCY_EXECUTOR_MOCK_H
