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
#ifndef SCORE_LIB_CONCURRENCY_INTERRUPTIBLEWAIT_H
#define SCORE_LIB_CONCURRENCY_INTERRUPTIBLEWAIT_H

#include "score/concurrency/condition_variable.h"

#include "score/stop_token.hpp"

#include <mutex>

namespace score
{
namespace concurrency
{

/**
 * \brief wait_for conditionally waits for stop_requested on token stop_source or expired timeout.
 */
template <typename TimeUnit>
bool wait_for(const score::cpp::stop_token& token, const TimeUnit& timeout)
{
    std::mutex mtx;
    InterruptibleConditionalVariable cv;
    std::unique_lock<std::mutex> lock{mtx};

    return cv.wait_for(lock, token, timeout, [&token]() {
        return token.stop_requested();
    });
}

/**
 * \brief wait_until conditionally waits for stop_requested on token stop_source or time_point being reached.
 */
template <typename TimeUnit>
bool wait_until(const score::cpp::stop_token& token, const TimeUnit& time_point)
{
    std::mutex mtx;
    InterruptibleConditionalVariable cv;
    std::unique_lock<std::mutex> lock{mtx};

    return cv.wait_until(lock, token, time_point, [&token]() {
        return token.stop_requested();
    });
}

/**
 * \brief wait_until_stop_requested conditionally waits for stop_requested on token stop_source.
 *
 * \post Will block forever if there is no stop_requested on token stop_source.
 */
void wait_until_stop_requested(const score::cpp::stop_token& token);

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_INTERRUPTIBLEWAIT_H
