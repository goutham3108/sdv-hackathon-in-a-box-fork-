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
#ifndef SCORE_LIB_SAFE_MATH_DETAILS_DIVISION_DIVISION_H
#define SCORE_LIB_SAFE_MATH_DETAILS_DIVISION_DIVISION_H

#include "score/language/safecpp/safe_math/details/absolute/absolute.h"
#include "score/language/safecpp/safe_math/details/cast/cast.h"
#include "score/language/safecpp/safe_math/details/floating_point_environment.h"
#include "score/language/safecpp/safe_math/details/negate/negate.h"
#include "score/language/safecpp/safe_math/details/type_traits/type_traits.h"
#include "score/language/safecpp/safe_math/error.h"
#include "score/language/safecpp/safe_math/return_mode.h"

#include "score/result/result.h"

#include <cstdlib>
#include <limits>
#include <type_traits>

namespace score::safe_math
{

// We use this template trick to make an intelligent default choice for the return type while allowing a user to
// specify the return type without having to specify the parameter types.
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class LHS,
          class RHS,
          class R = prefer_first_type_t<TempR, LHS>,
          typename std::enable_if_t<std::is_integral<LHS>::value && std::is_integral<RHS>::value &&
                                        std::is_integral<R>::value,
                                    bool> = true>
auto Divide(LHS lhs, RHS rhs) noexcept -> ModeBasedReturnType<R, return_mode>
{
    if (CmpEqual(rhs, 0))
    {
        return HandleError<R, return_mode>(score::MakeUnexpected(ErrorCode::kDivideByZero));
    }

    using UnsignedLHS = std::make_unsigned_t<LHS>;
    using UnsignedRHS = std::make_unsigned_t<RHS>;
    using CommonType = bigger_type_t<UnsignedLHS, UnsignedRHS>;

    const auto abs_lhs = static_cast<CommonType>(Abs(lhs));
    const auto abs_rhs = static_cast<CommonType>(Abs(rhs));

    if ((abs_lhs % abs_rhs) != 0U)
    {
        return HandleError<R, return_mode>(score::MakeUnexpected(ErrorCode::kImplicitRounding));
    }
    const auto result = static_cast<CommonType>(abs_lhs / abs_rhs);

    if (CmpLess(lhs, 0) == CmpLess(rhs, 0))
    {
        return Cast<return_mode, R>(result);
    }
    else
    {
        return Negate<return_mode, R>(result);
    }
}

template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class FloatingLHS,
          class FloatingRHS,
          class R = prefer_first_type_t<TempR, bigger_type_t<FloatingLHS, FloatingRHS>>,
          typename std::enable_if_t<std::is_floating_point<FloatingLHS>::value &&
                                        std::is_floating_point<FloatingRHS>::value && std::is_floating_point<R>::value,
                                    bool> = true>
auto Divide(FloatingLHS lhs_floating, FloatingRHS rhs_floating) noexcept -> ModeBasedReturnType<R, return_mode>
{
    if (rhs_floating == 0.0)
    {
        // This goes against IEEE 754 but is the mathematical correct solution.
        return HandleError<R, return_mode>(score::MakeUnexpected(ErrorCode::kDivideByZero));
    }

    static_assert(std::numeric_limits<FloatingLHS>::is_iec559 && std::numeric_limits<FloatingRHS>::is_iec559,
                  "Operands must adhere to IEEE 754 for ensured accuracy of results");

    return details::FloatingPointEnvironment::CalculateAndVerify<return_mode>(
        // Suppress "AUTOSAR C++14 A7-1-7", The rule states: "Each expression statement and identifier
        // declaration shall be placed on a separate line." The code here is present in single line to avoid
        // clang format failure.
        // coverity[autosar_cpp14_a7_1_7_violation]
        [lhs_floating, rhs_floating]() {
            return lhs_floating / rhs_floating;
        });
}

template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class FloatingLHS,
          class IntegralRHS,
          class R = prefer_first_type_t<TempR, FloatingLHS>,
          typename std::enable_if_t<std::is_floating_point<FloatingLHS>::value &&
                                        std::is_integral<IntegralRHS>::value && std::is_floating_point<R>::value,
                                    bool> = true>
// Suppress "UNCAUGHT_EXCEPT" rule findings. This rule states: "Called function throws an exception of type
// std::bad_variant_access".
// Rationale: There is no code path where the exception would be thrown.
// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// implicitly.".
// Rationale: There is no code path where std::terminate would be implicitly called.
// coverity[fun_call_w_exception]
// coverity[uncaught_exception]
// coverity[autosar_cpp14_a15_5_3_violation]
auto Divide(FloatingLHS lhs_floating, IntegralRHS rhs_integral) noexcept -> ModeBasedReturnType<R, return_mode>
{
    const auto rhs_floating = Cast<return_mode, R>(rhs_integral);

    const auto divide = [&lhs_floating](R rhs_casted) noexcept -> ModeBasedReturnType<R, return_mode> {
        return Divide<return_mode, R>(lhs_floating, rhs_casted);
    };
    return PerformActionBasedOnReturnMode<return_mode, R>(divide, rhs_floating);
}

template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class IntegralLHS,
          class FloatingRHS,
          class R = prefer_first_type_t<TempR, FloatingRHS>,
          typename std::enable_if_t<std::is_integral<IntegralLHS>::value &&
                                        std::is_floating_point<FloatingRHS>::value && std::is_floating_point<R>::value,
                                    bool> = true>
// Suppress "UNCAUGHT_EXCEPT" rule findings. This rule states: "Called function throws an exception of type
// std::bad_variant_access".
// Rationale: There is no code path where the exception would be thrown.
// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// implicitly.".
// Rationale: There is no code path where std::terminate would be implicitly called.
// coverity[fun_call_w_exception]
// coverity[uncaught_exception]
// coverity[autosar_cpp14_a15_5_3_violation]
auto Divide(IntegralLHS lhs_integral, FloatingRHS rhs_floating) noexcept -> ModeBasedReturnType<R, return_mode>
{
    const auto lhs_floating = Cast<return_mode, R>(lhs_integral);

    const auto divide = [&rhs_floating](R lhs_casted) noexcept -> ModeBasedReturnType<R, return_mode> {
        return Divide<return_mode, R>(lhs_casted, rhs_floating);
    };
    return PerformActionBasedOnReturnMode<return_mode, R>(divide, lhs_floating);
}

// Type-first convenience overload: allows specifying only the return type (TempR) without explicitly restating the
// default ReturnMode.
template <class TempR, class LHS, class RHS>
auto Divide(LHS lhs, RHS rhs) noexcept -> decltype(Divide<kDefaultReturnMode, TempR>(lhs, rhs))
{
    return Divide<kDefaultReturnMode, TempR>(lhs, rhs);
}

}  // namespace score::safe_math

#endif  // SCORE_LIB_SAFE_MATH_DETAILS_DIVISION_DIVISION_H
