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
#include "score/concurrency/long_running_threads_container.h"

namespace score
{
namespace concurrency
{

LongRunningThreadsContainer::~LongRunningThreadsContainer() noexcept
{
    InternalShutdown();
}

void LongRunningThreadsContainer::Shutdown() noexcept
{
    InternalShutdown();
}

void LongRunningThreadsContainer::InternalShutdown() noexcept
{
    std::unique_lock<std::mutex> lock{mutex_};
    for (auto& task_stop_source : active_)
    {
        score::cpp::ignore = task_stop_source.request_stop();
    }
    score::cpp::ignore = stop_source_.request_stop();
}

bool LongRunningThreadsContainer::ShutdownRequested() const noexcept
{
    return stop_source_.stop_requested();
}

std::size_t LongRunningThreadsContainer::MaxConcurrencyLevel() const noexcept
{
    return pool_.max_size();
}

void LongRunningThreadsContainer::Enqueue(score::cpp::pmr::unique_ptr<Task> task)
{
    std::unique_lock<std::mutex> lock{mutex_};
    if (stop_source_.stop_requested())
    {
        (*task)(stop_source_.get_token());
    }
    else
    {
        score::cpp::ignore = active_.emplace_back(task->GetStopSource());
        score::cpp::ignore = pool_.emplace_back([task = std::move(task)]() {
            (*task)(task->GetStopSource().get_token());
        });
    }
}

}  // namespace concurrency
}  // namespace score
