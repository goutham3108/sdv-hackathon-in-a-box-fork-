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
#include "score/os/utils/path_impl.h"

#include <process.h>

#include <climits>
#include <string>
#include <system_error>
#include <vector>

// Suppress "AUTOSAR C++14 A15-5-3" rule findings: "The std::terminate() function shall not be called implicitly".
// Rationale: Calling std::terminate() if any exceptions are thrown is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
score::cpp::expected<std::string, score::os::Error> score::os::PathImpl::get_exec_path() const noexcept
{

    constexpr const std::size_t path_max{static_cast<std::size_t>(PATH_MAX)};
    constexpr const std::size_t null_termination{1U};
    std::vector<char> vec_result(path_max + null_termination);

    score::cpp::expected<std::string, score::os::Error> result{""};

    // Function returns NULL only if the pathname of the executing program cannot be determined. There is no reliable
    // way to simulate above error cases within the scope of a unit test.
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-qnx-banned-builtin) see comment above
    if (nullptr != _cmdname(vec_result.data()))  // LCOV_EXCL_BR_LINE
    {
        result = std::string(vec_result.data(), vec_result.size());
    }
    else
    {
        // _cmdname does not set errno nor returns any error code. To satisfy the
        // interface, 'kUnexpected' is set since it is a recommended alternative to use
        // when an error appears that is not represented as OS agnostic. See: errno.h
        result = score::cpp::make_unexpected(score::os::Error::createUnspecifiedError());  // LCOV_EXCL_LINE
    }
    return result;
}
