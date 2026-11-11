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
#ifndef SCORE_LIB_OS_LIBGEN_IMPL_H
#define SCORE_LIB_OS_LIBGEN_IMPL_H

#include "score/os/libgen.h"
#include <libgen.h>

namespace score
{
namespace os
{

class LibgenImpl final : public Libgen
{
  public:
    constexpr LibgenImpl() = default;

  private:
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operations */
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    char* base_name(char* const path) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operations */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    char* dirname(char* const path) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
};

}  // namespace os
}  // namespace score

#endif
