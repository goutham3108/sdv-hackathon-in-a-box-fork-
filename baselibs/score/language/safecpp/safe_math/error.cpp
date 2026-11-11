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
#include "score/language/safecpp/safe_math/error.h"

std::string_view score::safe_math::ErrorDomain::MessageFor(const score::result::ErrorCode& code) const noexcept
{
    using SafeMathErrorCode = score::safe_math::ErrorCode;
    using ResultErrorCode = std::decay_t<decltype(code)>;
    static_assert(std::is_same_v<ResultErrorCode, std::underlying_type_t<SafeMathErrorCode>>);

    // Suppress "AUTOSAR C++14 A7-2-1", The rule states: "An expression with enum underlying type shall only have
    // values corresponding to the enumerators of the enumeration." This is a false positive.
    // Suppress "AUTOSAR C++14 M6-4-5" and "AUTOSAR C++14 M6-4-3", The rule states: An unconditional throw or break
    // statement shall terminate every nonempty switch-clause." and "A switch statement shall be a well-formed switch
    // statement.", respectively.The `return` statement in this case clause unconditionally exits the function, making
    // an additional `break` statement redundant.
    // coverity[autosar_cpp14_m6_4_3_violation]
    // coverity[autosar_cpp14_a7_2_1_violation]
    switch (static_cast<SafeMathErrorCode>(code))
    {
        // coverity[autosar_cpp14_m6_4_5_violation]
        case ErrorCode::kExceedsNumericLimits:
            return "Operation exceeds numeric limits";
        // coverity[autosar_cpp14_m6_4_5_violation]
        case ErrorCode::kImplicitRounding:
            return "Operation implicitly rounds result";
        // coverity[autosar_cpp14_m6_4_5_violation]
        case ErrorCode::kDivideByZero:
            return "Division by zero";
        // coverity[autosar_cpp14_m6_4_5_violation]
        case ErrorCode::kUnknown:
        default:
            return "Unknown error";
    }
}

namespace
{
constexpr score::safe_math::ErrorDomain my_safe_math_error_domain;
}

score::result::Error score::safe_math::MakeError(const ErrorCode code, const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), my_safe_math_error_domain, user_message};
}
