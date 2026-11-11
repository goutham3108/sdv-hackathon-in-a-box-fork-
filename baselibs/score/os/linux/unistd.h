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
#ifndef SCORE_LIB_OS_LINUX_UNISTD_H
#define SCORE_LIB_OS_LINUX_UNISTD_H

#include <unistd.h>
#include <cstdint>
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
namespace score
{
namespace os
{
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: not hides unistd.h, function wrapper within namespace */
std::int32_t daemon(const std::int32_t nochdir, const std::int32_t noclose);
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN */

/* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Wrapped function requires C-style array param */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: not hides unistd.h, function wrapper within namespace */
std::int32_t pipe2(std::int32_t pipedes[2], const std::int32_t flags);
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_LINUX_UNISTD_H
