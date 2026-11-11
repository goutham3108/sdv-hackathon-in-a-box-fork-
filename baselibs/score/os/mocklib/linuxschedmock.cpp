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
#include <score/callback.hpp>

#include "linuxschedmock.h"

namespace score
{
namespace os
{

namespace
{
score::cpp::callback<std::int32_t(void)> sched_getcpu_callback;
} /* namespace */

LinuxSchedMock::LinuxSchedMock()
{
    sched_getcpu_callback = [this]() -> std::uint32_t {
        return this->sched_getcpu_os();
    };
}

std::int32_t sched_getcpu_os()
{
    return sched_getcpu_callback();
}

} /* namespace os */
} /* namespace score */
