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
#ifndef SCORE_LIB_CONCURRENCY_INTERRUPTIBLE_CONDITIONAL_VARIABLE_MOCK_H
#define SCORE_LIB_CONCURRENCY_INTERRUPTIBLE_CONDITIONAL_VARIABLE_MOCK_H

#include "score/concurrency/clock.h"

#include <score/callback.hpp>
#include <score/stop_token.hpp>

#include <gmock/gmock.h>

#include <mutex>

namespace score::concurrency::testing
{
class InterruptibleConditionalVariableMock
{
  public:
    MOCK_METHOD(bool,
                wait_until,
                (std::unique_lock<std::mutex>&,
                 const score::cpp::stop_token&,
                 const score::concurrency::testing::SteadyClock::time_point&,
                 const score::cpp::callback<bool()>&)

    );
};
}  // namespace score::concurrency::testing

#endif  // SCORE_LIB_CONCURRENCY_INTERRUPTIBLE_CONDITIONAL_VARIABLE_MOCK_H
