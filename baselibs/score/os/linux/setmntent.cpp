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
#include "score/os/linux/setmntent.h"

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Respect wrapper function signature */
FILE* setMntent(const char* const file, const char* const mode)
{
    return ::setmntent(file, mode);
}
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Respect wrapper function signature */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function */

} /* namespace os */
} /* namespace score */
