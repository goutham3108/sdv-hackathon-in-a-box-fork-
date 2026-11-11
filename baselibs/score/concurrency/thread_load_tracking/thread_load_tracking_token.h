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
#ifndef THREAD_LOAD_TRACKING_TOKEN_H
#define THREAD_LOAD_TRACKING_TOKEN_H

#include "score/concurrency/thread_load_tracking/thread_load_tracking_state.h"

#include <chrono>

namespace score
{
namespace concurrency
{

class ThreadLoadTracking;

/// \brief RAII work and wait tracking token.
class ThreadLoadTrackingToken
{
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Force "ThreadLoadTrackingToken" to be created only with "ThreadLoadTracking"
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend ThreadLoadTracking;

  public:
    /// \brief Method to early stopping the tracking before the destructor is called.
    void End() noexcept;

    /// \brief Stops the tracking if it is not already stopped by calling End().
    ~ThreadLoadTrackingToken() noexcept;

    // RAII object shall be only movable not copyable.
    ThreadLoadTrackingToken(const ThreadLoadTrackingToken&) = delete;
    ThreadLoadTrackingToken(ThreadLoadTrackingToken&&) = default;
    ThreadLoadTrackingToken& operator=(const ThreadLoadTrackingToken&) = delete;
    ThreadLoadTrackingToken& operator=(ThreadLoadTrackingToken&&) = default;

  private:
    ThreadLoadTrackingToken(ThreadLoadTracking&, const ThreadLoadTrackingState&) noexcept;

    ThreadLoadTracking* tracking_;
    ThreadLoadTrackingState state_;
    std::chrono::steady_clock::time_point start_time_;
    bool end_called_;
};

}  // namespace concurrency
}  // namespace score
#endif
