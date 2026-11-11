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
#ifndef SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_TIMED_TASK_H
#define SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_TIMED_TASK_H

#include "score/concurrency/task.h"

#include <optional>

namespace score::concurrency
{

/// @brief Sometimes tasks need to be executed in a specific timely manner. For example periodic, or at a certain time
/// point. The goal of this interface is to enable the implementation of tasks that are based on timings.
/// \tparam Clock Any clock following the named requirements of `TrivialClock` (e.g. steady_clock)
template <typename Clock>
class TimedTask : public Task
{
  public:
    TimedTask() = default;
    TimedTask(const TimedTask&) = delete;
    TimedTask& operator=(const TimedTask&) = delete;

  protected:
    TimedTask(TimedTask&&) noexcept = default;
    TimedTask& operator=(TimedTask&&) noexcept = default;

  public:
    ~TimedTask() override = default;

    /// @brief Retrieve the time point when this task should be executed next
    /// @note The time point is permitted to change, e.g. after the task got (re-)executed
    /// @return returns no value if TimedTask should not be rescheduled, the time point when to execute the task
    /// otherwise
    [[nodiscard]] virtual auto GetNextExecutionPoint() const noexcept
        -> std::optional<std::chrono::time_point<Clock>> = 0;
};

}  // namespace score::concurrency

#endif  // SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_TIMED_TASK_H
