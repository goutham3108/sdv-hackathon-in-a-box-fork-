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

#include "score/os/libgen.h"
#include "score/os/unistd.h"

#include "score/assert.hpp"

#include <climits>
#include <string>
#include <system_error>
#include <vector>

score::cpp::expected<std::string, score::os::Error> score::os::PathImpl::get_exec_path() const noexcept
{
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
    constexpr const std::size_t path_max{PATH_MAX};
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION */
    constexpr const std::size_t null_termination{1U};
    std::vector<char> vec_result(path_max + null_termination);

    // TODO: Consider using score::os wrapper functions once TicketOld-70062 is resolved for:
    // readlink(), read(), errno,

    /* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used to store POSIX call return value */
    const std::string exe_path{"/proc/self/exe"};
    const auto ret = score::os::Unistd::instance().readlink(exe_path.c_str(), vec_result.data(), path_max);
    const ssize_t length{ret.has_value() ? ret.value() : -1};
    /* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used to store POSIX call return value */

    score::cpp::expected<std::string, score::os::Error> result{""};
    // LCOV_EXCL_START: Linux specific code, scope of codecoverage is only for qnx code
    if (length < 0)
    {
        /* KW_SUPPRESS_START:MISRA.EXPANSION.UNSAFE,MISRA.USE.EXPANSION: */
        /* Using library-defined macro to ensure correct operation */
        result = score::cpp::make_unexpected(score::os::Error::createFromErrno(errno));
        /* KW_SUPPRESS_END:MISRA.EXPANSION.UNSAFE,MISRA.USE.EXPANSION */
    }
    else if (static_cast<std::size_t>(length) >= path_max)
    {
        /* KW_SUPPRESS_START:MISRA.USE.EXPANSION: Using library-defined macro to ensure correct operation */
        result = score::cpp::make_unexpected(score::os::Error::createFromErrno(ENAMETOOLONG));
        /* KW_SUPPRESS_END:MISRA.USE.EXPANSION */
    }
    // LCOV_EXCL_STOP
    else
    {
        vec_result[static_cast<std::vector<char>::size_type>(length)] = '\0';
        result = std::string(vec_result.data(), static_cast<std::size_t>(length));
    }
    return result;
}
