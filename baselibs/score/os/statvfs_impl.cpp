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
#include "score/os/statvfs_impl.h"

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{
/* KW_SUPPRESS_START: MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<Error> StatvfsImpl::statvfs(const char* const path, struct statvfs* const buf) const noexcept
/* KW_SUPPRESS_END: MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    if (::statvfs(path, buf) == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return {};
}
}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
