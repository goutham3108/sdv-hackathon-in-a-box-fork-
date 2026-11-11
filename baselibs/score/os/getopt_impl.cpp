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
#include "score/os/getopt_impl.h"

namespace score
{
namespace os
{
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:The wrapper function is identifiable through namespace */
/* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Wrapped function's signature requires C-style array */
// Wrapped function's signature requires C-style array
// NOLINTBEGIN(modernize-avoid-c-arrays) see comment above
// Suppress "AUTOSAR C++14 M0-1-3", The rule states: "Every function defined in an anonymous namespace,
// or static function with internal linkage, or private member function shall be used.
// Justification: These are wrappers and public in Base, derived as private to make it inaccessible except from base.
// coverity[autosar_cpp14_a0_1_3_violation]
std::int32_t GetoptImpl::getopt(const std::int32_t argc, char* const argv[], const char* const optstring) const noexcept
// NOLINTEND(modernize-avoid-c-arrays)
/* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE:Wrapped function's signature requires C-style array */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:The wrapper function is identifiable through namespace */
{
    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTNEXTLINE(score-banned-function) see comment above
    return ::getopt(argc, argv, optstring);
}

// coverity[autosar_cpp14_a0_1_3_violation] see justification above
std::int32_t GetoptImpl::getoptopt() const noexcept
{
    return optopt;
}

// coverity[autosar_cpp14_a0_1_3_violation] see justification above
std::int32_t GetoptImpl::getoptind() const noexcept
{
    return optind;
}

// coverity[autosar_cpp14_a0_1_3_violation] see justification above
std::int32_t GetoptImpl::getopterr() const noexcept
{
    return opterr;
}
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:The wrapper function is identifiable through namespace */
/* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Wrapped function's signature requires C-style array */
// Wrapped function's signature requires C-style array
// coverity[autosar_cpp14_a0_1_3_violation] see justification above
std::int32_t GetoptImpl::getopt_long(const std::int32_t argc,
                                     char* const argv[],  // NOLINT(modernize-avoid-c-arrays) see comment above
                                     const char* const optstring,
                                     const option* const option,
                                     std::int32_t* const index) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:The wrapper function is identifiable through namespace */
/* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE:Wrapped function's signature requires C-style array */
{
    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTNEXTLINE(score-banned-function) see comment above
    return ::getopt_long(argc, argv, optstring, option, index);
}
}  // namespace os
}  // namespace score
