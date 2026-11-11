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
#ifndef SCORE_LIB_SAFE_MATH_ERROR_H
#define SCORE_LIB_SAFE_MATH_ERROR_H

#include "score/result/error.h"
#include "score/result/error_code.h"
#include "score/result/error_domain.h"

namespace score::safe_math
{
enum class ErrorCode : score::result::ErrorCode
{
    kUnknown = 0,  // Value for default initialization - never returned on purpose
    kExceedsNumericLimits,
    kImplicitRounding,
    kDivideByZero,
};

class ErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override;
};

score::result::Error MakeError(const ErrorCode code, const std::string_view user_message = "") noexcept;

}  // namespace score::safe_math
#endif  // SCORE_LIB_SAFE_MATH_ERROR_H
