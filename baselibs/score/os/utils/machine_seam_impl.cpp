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
#include "score/os/utils/machine_seam_impl.h"

namespace score
{
namespace os
{

bool MachineImpl::is_qemu() const noexcept
{
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef MACHINE_QEMU
    return true;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
    // LCOV_EXCL_START
    // Below code is excluded from lcov (line and branch) coverage, as it involves machine-specific functionality that
    // cannot be tested within the current unit testing environment because of dependency on Specific Hardware. Unit
    // tests are executed  on QEMU but not in hardware. Hence "else" case corresponds to a scenario that is specific to
    // hardware cannot be simulated accurately in QEMU due to limitations in emulation.
    return false;
    // LCOV_EXCL_STOP
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
}

}  // namespace os
}  // namespace score
