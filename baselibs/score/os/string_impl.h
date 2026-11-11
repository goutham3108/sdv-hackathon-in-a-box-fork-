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
#ifndef SCORE_LIB_OS_STRING_IMPL_H
#define SCORE_LIB_OS_STRING_IMPL_H

#include "score/os/string.h"

namespace score
{
namespace os
{

class StringImpl : public String
{
  public:
    constexpr StringImpl() = default;
    /* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: memcpy is a wrapper method to call OS method strerror */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    void* memcpy(void* const dest, const void* const src, const std::size_t n) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME:*/

    /* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: strerror is a wrapper method to call OS method strerror */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    char* strerror(const std::int32_t errnum) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME:*/

    /* KW_SUPPRESS_START:MISRA.STDLIB.WRONGNAME: memset is a wrapper method to call OS method strerror */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    void* memset(void* const dest, const std::int32_t c, const std::size_t n) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.STDLIB.WRONGNAME:*/
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_STRING_IMPL_H
