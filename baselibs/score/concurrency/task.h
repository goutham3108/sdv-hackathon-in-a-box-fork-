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
#ifndef SCORE_LIB_CONCURRENCY_TASK_H
#define SCORE_LIB_CONCURRENCY_TASK_H

#include <score/apply.hpp>
#include <score/stop_token.hpp>

#include <future>
#include <utility>

namespace score
{
namespace concurrency
{

/**
 * \brief This interface wraps a task for usage in an Executor
 */
class Task
{
  public:
    Task() = default;
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

  protected:
    Task(Task&&) = default;
    Task& operator=(Task&&) = default;

  public:
    virtual ~Task() = default;

    /**
     * \brief Calls either underlying Callable
     * and sets promise if applicable
     *
     * \param executor the executor running this task
     * \param token indicates whether the underlying task shall be stopped
     */
    virtual void operator()(const score::cpp::stop_token token) = 0;

    /**
     * \brief Returns the stop source of this task that can be used to stop this
     * task.
     *
     * \return The stop source
     */
    virtual score::cpp::stop_source GetStopSource() const noexcept = 0;
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_TASK_H
