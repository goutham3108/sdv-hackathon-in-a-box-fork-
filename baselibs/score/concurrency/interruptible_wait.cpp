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
#include "score/concurrency/interruptible_wait.h"

#include "score/utility.hpp"

void score::concurrency::wait_until_stop_requested(const score::cpp::stop_token& token)
{
    std::mutex mtx;
    InterruptibleConditionalVariable cv;
    std::unique_lock<std::mutex> lock(mtx);

    score::cpp::ignore = cv.wait(lock, token, []() {
        return false;
    });
}
