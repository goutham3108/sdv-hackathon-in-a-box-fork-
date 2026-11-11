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
#include "score/os/utils/thread.h"

#include <sys/neutrino.h>

// https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.prog/topic/multicore_thread_affinity.html
bool score::os::set_thread_affinity(const std::size_t cpu) noexcept
{
    // https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.lib_ref/topic/t/threadctl.html#threadctl___NTO_TCTL_RUNMASK_GET_AND_SET
    // No harm from implicit integral conversion here
    // coverity[autosar_cpp14_m5_0_3_violation]
    // coverity[autosar_cpp14_m5_0_10_violation]
    // coverity[autosar_cpp14_a4_7_1_violation]
    std::uint32_t runmask = 0x1U << static_cast<std::uint32_t>(cpu);
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t result = ThreadCtl(_NTO_TCTL_RUNMASK_GET_AND_SET, &runmask);
    return (result != -1);
}
