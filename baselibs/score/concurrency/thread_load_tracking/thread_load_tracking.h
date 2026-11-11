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
#ifndef THREAD_LOAD_TRACKING_H
#define THREAD_LOAD_TRACKING_H

#include "score/concurrency/thread_load_tracking/thread_load_tracking_token.h"
#include "score/concurrency/thread_load_tracking/work_load.h"

#include "score/callback.hpp"

#include <atomic>
#include <chrono>
#include <mutex>

namespace score
{
namespace concurrency
{

/// \brief Tracks the work load of a thread i.e. the ratio of working to waiting.
/// \details This utility class can be used to estimate the work load of a single thread. We classify the behavior of
/// the thread under test into "working" and "waiting". Thus the class is a good fit for threads that idle until some
/// data is available, and then process the data until done.
/// \note The public interface of this class shall be thread safe.
class ThreadLoadTracking
{

    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Encapsultes the need to call "get_time_now_()" before creating and ending of "ThreadLoadTrackingToken" to measure
    // the processing duration.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend ThreadLoadTrackingToken;

  public:
    /// \brief Returns a token to track working time.
    ThreadLoadTrackingToken StartWorking() noexcept;

    /// \brief Returns a token to track waiting time.
    ThreadLoadTrackingToken StartWaiting() noexcept;

    /// \brief Calculates the current work load value.
    WorkLoad Calculate() noexcept;

    // Allow the injection of the now function for unit testing.
    using GetTimePointFunction = score::cpp::callback<std::chrono::steady_clock::time_point(void)>;
    explicit ThreadLoadTracking(GetTimePointFunction get_time_now = &std::chrono::steady_clock::now);

    ~ThreadLoadTracking() = default;

    using TrackingResolution = std::chrono::steady_clock::duration;

    // Class must not be moved or copied because the token stores a reference to the tracking instance.
    ThreadLoadTracking(const ThreadLoadTracking&) = delete;
    ThreadLoadTracking(ThreadLoadTracking&&) = delete;
    ThreadLoadTracking& operator=(const ThreadLoadTracking&) = delete;
    ThreadLoadTracking& operator=(ThreadLoadTracking&&) = delete;

  private:
    /// \brief Method to be called by ThreadLoadTrackingToken.
    /// \arg update_work_duration if false wait duration will be updated.
    void OnTokenEnd(const TrackingResolution& duration, const ThreadLoadTrackingState& state) noexcept;

    /// \brief Increase the work duration tracker.
    void TrackWorkDuration(const TrackingResolution& duration, std::unique_lock<std::mutex>&& lock) noexcept;

    /// \brief Increase the wait duration tracker.
    void TrackWaitDuration(const TrackingResolution& duration, std::unique_lock<std::mutex>&& lock) noexcept;

    mutable std::mutex mutex_;
    TrackingResolution wait_duration_;
    TrackingResolution work_duration_;
    GetTimePointFunction get_time_now_;
};

}  // namespace concurrency
}  // namespace score

#endif
