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

#include "score/math.hpp"

namespace score
{
namespace concurrency
{

ThreadLoadTracking::ThreadLoadTracking(GetTimePointFunction get_time_now)
    : mutex_{}, wait_duration_{}, work_duration_{}, get_time_now_{std::move(get_time_now)}
{
}

ThreadLoadTrackingToken ThreadLoadTracking::StartWorking() noexcept
{
    return {*this, ThreadLoadTrackingState::kWorking};
}

ThreadLoadTrackingToken ThreadLoadTracking::StartWaiting() noexcept
{
    return {*this, ThreadLoadTrackingState::kWaiting};
}

void ThreadLoadTracking::OnTokenEnd(const TrackingResolution& duration, const ThreadLoadTrackingState& state) noexcept
{
    std::unique_lock<std::mutex> lock{mutex_};
    if (state == ThreadLoadTrackingState::kWorking)
    {
        TrackWorkDuration(duration, std::move(lock));
    }
    else
    {
        TrackWaitDuration(duration, std::move(lock));
    }
}

void ThreadLoadTracking::TrackWorkDuration(const TrackingResolution& duration, std::unique_lock<std::mutex>&&) noexcept
{
    work_duration_ += duration;
}

void ThreadLoadTracking::TrackWaitDuration(const TrackingResolution& duration, std::unique_lock<std::mutex>&&) noexcept
{
    wait_duration_ += duration;
}

WorkLoad ThreadLoadTracking::Calculate() noexcept
{
    std::lock_guard<std::mutex> lock{mutex_};

    // coverity[autosar_cpp14_m8_5_2_violation] kept for the sake of zero-initialization
    WorkLoad result{};

    const auto work_fp = work_duration_.count();
    const auto wait_fp = wait_duration_.count();
    const auto work_and_wait_duration = work_fp + wait_fp;

    if (work_and_wait_duration != 0)
    {
        result.work_load_percent = 100.0 * static_cast<double>(work_fp) / static_cast<double>(work_and_wait_duration);
    }

    result.work_duration = work_duration_;
    result.wait_duration = wait_duration_;

    // Reset duration counters
    work_duration_ = TrackingResolution{};
    wait_duration_ = TrackingResolution{};

    return result;
}

}  // namespace concurrency
}  // namespace score
