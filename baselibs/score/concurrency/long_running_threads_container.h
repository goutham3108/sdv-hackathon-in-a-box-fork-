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
#ifndef SCORE_LIB_CONCURRENCY_LONG_RUNNING_THREADS_CONTAINER_H
#define SCORE_LIB_CONCURRENCY_LONG_RUNNING_THREADS_CONTAINER_H

#include "score/concurrency/executor.h"
#include "score/concurrency/task.h"

#include <score/jthread.hpp>
#include <score/stop_token.hpp>

namespace score
{
namespace concurrency
{

/// @brief In difference to ThreadPool, this class automatically creates new threads for each enqueued task.
///        The idea is, that enqueued tasks _blocking_ tasks - meaning, that most of the time they wait on
///        a custom condition and only then perform small amounts of work. Threads will not be reused.
class LongRunningThreadsContainer final : public Executor
{
  public:
    LongRunningThreadsContainer() = default;
    ~LongRunningThreadsContainer() noexcept override;
    std::size_t MaxConcurrencyLevel() const noexcept override;
    bool ShutdownRequested() const noexcept override;
    void Shutdown() noexcept override;
    void Enqueue(score::cpp::pmr::unique_ptr<Task> task) override;

    LongRunningThreadsContainer(const LongRunningThreadsContainer&) = delete;
    LongRunningThreadsContainer(LongRunningThreadsContainer&&) noexcept = delete;
    LongRunningThreadsContainer& operator=(const LongRunningThreadsContainer&) = delete;
    LongRunningThreadsContainer& operator=(LongRunningThreadsContainer&&) noexcept = delete;

  private:
    void InternalShutdown() noexcept;

    std::mutex mutex_{};
    std::vector<score::cpp::stop_source> active_{};
    score::cpp::stop_source stop_source_{};

    // This is intentionally last, since this will ensure that on destruction we first wait for all threads to stop
    // before we destruct anything else.
    std::vector<score::cpp::jthread> pool_{};
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_LONG_RUNNING_THREADS_CONTAINER_H
