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
#ifndef SCORE_LIB_OS_UTILS_HIGH_RESOLUTION_STEADY_CLOCK_H
#define SCORE_LIB_OS_UTILS_HIGH_RESOLUTION_STEADY_CLOCK_H

#include <chrono>

namespace score
{
namespace os
{

///
/// \brief std::chrono-compatible monotonic clock that offers a higher resolution than just milliseconds
///
/// \details On QNX, `std::chrono::steady_clock` only offers millisecond resolution. Hence, another clock must
///          be used under QNX to obtain a higher resolution. From QNX SDP 8.39.10 on, nanosecond resolution can
///          be acheived by using `std::chrono::high_resolution_clock` instead since it utilizes QNX's `ClockCycles()`
///          method (cf. https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/c/clockcycles.html)
///          which reads the TSC counter of the Intel Denverton Processor (mPAD). According to Intel's Safety Manual,
///          for the Denverton Processor, such counter is stable and also offers integrity according to ASIL-B.
///          Further details are also contained in ticket broken_link_ac/jira/browse/TicketOld-82361.
///
class HighResolutionSteadyClock
{
  public:
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef __QNX__
    using UnderlyingClock = std::chrono::high_resolution_clock;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
    using UnderlyingClock = std::chrono::steady_clock;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    // Suppress "AUTOSAR C++14 A0-1-1" rule finding. This rule states: "A project shall not contain instances of
    // non-volatile variables being given values that are not subsequently used.".
    // is_steady is used as static-initialized like score::os::HighResolutionSteadyClock::is_steady
    // coverity[autosar_cpp14_a0_1_1_violation]
    constexpr static bool is_steady = UnderlyingClock::is_steady;
    static_assert(is_steady, "HighResolutionSteadyClock's underlying clock must be a monotonic one");

    using rep = UnderlyingClock::rep;
    using period = UnderlyingClock::period;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<HighResolutionSteadyClock>;

    static std::chrono::time_point<HighResolutionSteadyClock> now() noexcept
    {
        return std::chrono::time_point<HighResolutionSteadyClock>{UnderlyingClock::now().time_since_epoch()};
    }
};

}  // namespace os
}  // namespace score

#endif  // #ifndef SCORE_LIB_OS_UTILS_HIGH_RESOLUTION_STEADY_CLOCK_H
