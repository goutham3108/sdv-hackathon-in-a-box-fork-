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
#include "score/os/string_impl.h"

#include <cstring>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: memcpy is a wrapper method to call OS method memcpy */
void* StringImpl::memcpy(void* const dest, const void* const src, const std::size_t n) const noexcept
{ /*KW_SUPPRESS_END: MISRA.STDLIB.WRONGNAME:*/
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    return ::memcpy(dest, src, n);
}

/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: strerror is a wrapper method to call OS method strerror */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
char* StringImpl::strerror(const std::int32_t errnum) const noexcept
{ /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME:*/
    return ::strerror(errnum);
}

/* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: memset is a wrapper method to call OS method memset */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
void* StringImpl::memset(void* const dest, const std::int32_t c, const std::size_t n) const noexcept
{
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME:*/
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    return ::memset(dest, c, n);
}

}  // namespace os
}  // namespace score
