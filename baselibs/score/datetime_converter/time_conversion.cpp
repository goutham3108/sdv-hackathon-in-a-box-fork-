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
#include "score/datetime_converter/time_conversion.h"

namespace score
{
namespace common
{
timespec timeout_in_timespec(const std::chrono::milliseconds timeout,
                             const std::chrono::time_point<std::chrono::system_clock> current_time)
{
    const std::chrono::nanoseconds time_to_wait = current_time.time_since_epoch() + timeout;
    return duration_to_timespec(time_to_wait);
}

}  // namespace common
}  // namespace score
