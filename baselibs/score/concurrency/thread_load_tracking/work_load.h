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
#ifndef SCORE_LIB_CONCURRENCY_THREAD_LOAD_TRACKING_WORK_LOAD_H
#define SCORE_LIB_CONCURRENCY_THREAD_LOAD_TRACKING_WORK_LOAD_H

#include <chrono>
#include <optional>

namespace score
{
namespace concurrency
{

/// \brief Represents the work load of a thread at one point in time.
struct WorkLoad
{
    /// \brief The time the thread spent working in the observed time frame.
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::chrono::nanoseconds work_duration{};

    /// \brief The time the thread spent waiting or idling in the observed time frame.
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::chrono::nanoseconds wait_duration{};

    /// \brief The ratio of the work duration to the overall duration, i.e. the sum of work and wait duration.
    /// Empty if work and wait duration are empty.
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::optional<double> work_load_percent{};
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_THREAD_LOAD_TRACKING_WORK_LOAD_H
