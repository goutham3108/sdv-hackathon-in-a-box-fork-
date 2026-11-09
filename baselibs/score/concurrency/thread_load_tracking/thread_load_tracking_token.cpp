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
#include "score/concurrency/thread_load_tracking/thread_load_tracking_token.h"

#include "score/concurrency/thread_load_tracking/thread_load_tracking.h"

namespace score
{
namespace concurrency
{

ThreadLoadTrackingToken::ThreadLoadTrackingToken(ThreadLoadTracking& tracking,
                                                 const ThreadLoadTrackingState& state) noexcept
    : tracking_{&tracking}, state_{state}, start_time_{tracking.get_time_now_()}, end_called_{false}
{
}

void ThreadLoadTrackingToken::End() noexcept
{
    // Check if End() was already called before.
    if (end_called_)
    {
        return;
    }
    end_called_ = true;

    const auto now = tracking_->get_time_now_();
    const auto duration = std::chrono::duration_cast<ThreadLoadTracking::TrackingResolution>(now - start_time_);
    tracking_->OnTokenEnd(duration, state_);
}

ThreadLoadTrackingToken::~ThreadLoadTrackingToken() noexcept
{
    End();
}

}  // namespace concurrency
}  // namespace score
