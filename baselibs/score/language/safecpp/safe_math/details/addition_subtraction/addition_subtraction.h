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
#ifndef SCORE_LIB_SAFE_MATH_DETAILS_ADDITION_SUBTRACTION_ADDITION_SUBTRACTION_H
#define SCORE_LIB_SAFE_MATH_DETAILS_ADDITION_SUBTRACTION_ADDITION_SUBTRACTION_H

#include "score/language/safecpp/safe_math/details/absolute/absolute.h"
#include "score/language/safecpp/safe_math/details/cast/cast.h"
#include "score/language/safecpp/safe_math/details/comparison/comparison.h"
#include "score/language/safecpp/safe_math/details/floating_point_environment.h"
#include "score/language/safecpp/safe_math/details/negate/negate.h"
#include "score/language/safecpp/safe_math/error.h"
#include "score/language/safecpp/safe_math/return_mode.h"

#include "score/result/result.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>

namespace score::safe_math
{
// We use this template trick to make an intelligent default choice for the return type while allowing a user to
// specify the return type without having to specify the parameter types.
// Suppress "AUTOSAR C++14 M3-2-3", The rule states: "A type, object or function that is used in multiple translation
// units shall be declared in one and only one file." This is a false positive. The declarations are present in same
// file.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class UnsignedLHS,
          class UnsignedRHS,
          class R = prefer_first_type_t<TempR, bigger_type_t<UnsignedLHS, UnsignedRHS>>,
          typename std::enable_if_t<is_unsigned_integral<UnsignedLHS>::value &&
                                        is_unsigned_integral<UnsignedRHS>::value && std::is_integral<R>::value,
                                    bool> = true>
// We use this template trick to make an intelligent default choice for the return type while allowing a user to
// specify the return type without having to specify the parameter types.
auto Add(UnsignedLHS lhs_unsigned, UnsignedRHS rhs_unsigned) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class LHS,
          class SignedRHS,
          class R = prefer_first_type_t<TempR, LHS>,
          typename std::enable_if_t<std::is_integral<LHS>::value && is_signed_integral<SignedRHS>::value &&
                                        std::is_integral<R>::value,
                                    bool> = true>
// We use this template trick to make an intelligent default choice for the return type while allowing a user to
// specify the return type without having to specify the parameter types.
auto Add(LHS lhs, SignedRHS rhs_signed) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class SignedLHS,
          class UnsignedRHS,
          class R = prefer_first_type_t<TempR, SignedLHS>,
          typename std::enable_if_t<is_signed_integral<SignedLHS>::value && is_unsigned_integral<UnsignedRHS>::value &&
                                        std::is_integral<R>::value,
                                    bool> = true>
// We use this template trick to make an intelligent default choice for the return type while allowing a user to
// specify the return type without having to specify the parameter types.
auto Add(SignedLHS lhs_signed, UnsignedRHS rhs_unsigned) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class FloatingLHS,
          class FloatingRHS,
          class R = prefer_first_type_t<TempR, bigger_type_t<FloatingLHS, FloatingRHS>>,
          typename std::enable_if_t<std::is_floating_point<FloatingLHS>::value &&
                                        std::is_floating_point<FloatingRHS>::value && std::is_floating_point<R>::value,
                                    bool> = true>
auto Add(FloatingLHS lhs_floating, FloatingRHS rhs_floating) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class FloatingLHS,
          class IntegralRHS,
          class R = prefer_first_type_t<TempR, FloatingLHS>,
          typename std::enable_if_t<std::is_floating_point<FloatingLHS>::value &&
                                        std::is_integral<IntegralRHS>::value && std::is_floating_point<R>::value,
                                    bool> = true>
auto Add(FloatingLHS lhs_floating, IntegralRHS rhs_integral) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class IntegralLHS,
          class FloatingRHS,
          class R = prefer_first_type_t<TempR, FloatingRHS>,
          typename std::enable_if_t<std::is_integral<IntegralLHS>::value &&
                                        std::is_floating_point<FloatingRHS>::value && std::is_floating_point<R>::value,
                                    bool> = true>
// We use this template trick to make an intelligent default choice for the return type while allowing a user to
// specify the return type without having to specify the parameter types.
auto Add(IntegralLHS lhs_integral, FloatingRHS rhs_floating) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class LHS,
          class SignedRHS,
          class R = prefer_first_type_t<TempR, LHS>,
          typename std::enable_if_t<std::is_integral<LHS>::value && is_signed_integral<SignedRHS>::value &&
                                        std::is_integral<R>::value,
                                    bool> = true>
// We use this template trick to make an intelligent default choice for the return type while allowing a user to
// specify the return type without having to specify the parameter types.
auto Subtract(LHS lhs, SignedRHS rhs_signed) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class SignedLHS,
          class UnsignedRHS,
          class R = prefer_first_type_t<TempR, SignedLHS>,
          typename std::enable_if_t<is_signed_integral<SignedLHS>::value && is_unsigned_integral<UnsignedRHS>::value &&
                                        std::is_integral<R>::value,
                                    bool> = true>
// We use this template trick to make an intelligent default choice for the return type while allowing a user to
// specify the return type without having to specify the parameter types.
auto Subtract(SignedLHS lhs_signed, UnsignedRHS rhs_unsigned) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class UnsignedLHS,
          class UnsignedRHS,
          class R = prefer_first_type_t<TempR, UnsignedLHS>,
          typename std::enable_if_t<is_unsigned_integral<UnsignedLHS>::value &&
                                        is_unsigned_integral<UnsignedRHS>::value && std::is_integral<R>::value,
                                    bool> = true>
// We use this template trick to make an intelligent default choice for the return type while allowing a user to
// specify the return type without having to specify the parameter types.
auto Subtract(UnsignedLHS lhs_unsigned, UnsignedRHS rhs_unsigned) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class FloatingLHS,
          class FloatingRHS,
          class R = prefer_first_type_t<TempR, bigger_type_t<FloatingLHS, FloatingRHS>>,
          typename std::enable_if_t<std::is_floating_point<FloatingLHS>::value &&
                                        std::is_floating_point<FloatingRHS>::value && std::is_floating_point<R>::value,
                                    bool> = true>
auto Subtract(FloatingLHS lhs_floating, FloatingRHS rhs_floating) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class FloatingLHS,
          class IntegralRHS,
          class R = prefer_first_type_t<TempR, FloatingLHS>,
          typename std::enable_if_t<std::is_floating_point<FloatingLHS>::value &&
                                        std::is_integral<IntegralRHS>::value && std::is_floating_point<R>::value,
                                    bool> = true>
auto Subtract(FloatingLHS lhs_floating, IntegralRHS rhs_integral) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode = kDefaultReturnMode,
          class TempR = void,
          class IntegralLHS,
          class FloatingRHS,
          class R = prefer_first_type_t<TempR, FloatingRHS>,
          typename std::enable_if_t<std::is_integral<IntegralLHS>::value &&
                                        std::is_floating_point<FloatingRHS>::value && std::is_floating_point<R>::value,
                                    bool> = true>
auto Subtract(IntegralLHS lhs_integral, FloatingRHS rhs_floating) noexcept -> ModeBasedReturnType<R, return_mode>;

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class UnsignedLHS,
          class UnsignedRHS,
          class R,
          typename std::enable_if_t<is_unsigned_integral<UnsignedLHS>::value &&
                                        is_unsigned_integral<UnsignedRHS>::value && std::is_integral<R>::value,
                                    bool>>
// Suppress "UNCAUGHT_EXCEPT" rule findings. This rule states: "Called function throws an exception of type
// std::bad_variant_access".
// Rationale: There is no code path where the exception would be thrown.
// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// implicitly.".
// Rationale: There is no code path where std::terminate would be implicitly called.
// coverity[fun_call_w_exception]
// coverity[uncaught_exception]
// coverity[autosar_cpp14_a15_5_3_violation]
auto Add(UnsignedLHS lhs_unsigned, UnsignedRHS rhs_unsigned) noexcept -> ModeBasedReturnType<R, return_mode>
{
    const auto lhs_common_type = Cast<return_mode, R>(lhs_unsigned);
    const auto rhs_common_type = Cast<return_mode, R>(rhs_unsigned);

    if constexpr (return_mode == ReturnMode::kReturnResultOnError)
    {
        if ((!lhs_common_type.has_value()) || (!rhs_common_type.has_value()))
        {
            return score::MakeUnexpected(ErrorCode::kExceedsNumericLimits);
        }

        const auto min_allowed_value = static_cast<R>(std::numeric_limits<R>::max() - *rhs_common_type);
        if (CmpGreater(*lhs_common_type, min_allowed_value))
        {
            return score::MakeUnexpected(ErrorCode::kExceedsNumericLimits);
        }

        return static_cast<R>(*lhs_common_type + *rhs_common_type);
    }
    else if constexpr (return_mode == ReturnMode::kAbortOnError)
    {
        const auto min_allowed_value = static_cast<R>(std::numeric_limits<R>::max() - rhs_common_type);
        if (CmpGreater(lhs_common_type, min_allowed_value))
        {
            std::abort();
        }
        return static_cast<R>(lhs_common_type + rhs_common_type);
    }
    else
    {
        EmitCompilerDiagnosticForUnhandledEnumerator<return_mode>();
    }
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class LHS,
          class SignedRHS,
          class R,
          typename std::enable_if_t<std::is_integral<LHS>::value && is_signed_integral<SignedRHS>::value &&
                                        std::is_integral<R>::value,
                                    bool>>
auto Add(LHS lhs, SignedRHS rhs_signed) noexcept -> ModeBasedReturnType<R, return_mode>
{
    if (CmpLess(rhs_signed, 0))
    {
        // We actually subtract
        return Subtract<return_mode, R>(lhs, Abs(rhs_signed));
    }
    else
    {
        using UnsignedRHS = std::make_unsigned_t<SignedRHS>;
        const auto rhs_unsigned = static_cast<UnsignedRHS>(rhs_signed);
        return Add<return_mode, R>(lhs, rhs_unsigned);
    }
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class SignedLHS,
          class UnsignedRHS,
          class R,
          typename std::enable_if_t<is_signed_integral<SignedLHS>::value && is_unsigned_integral<UnsignedRHS>::value &&
                                        std::is_integral<R>::value,
                                    bool>>
auto Add(SignedLHS lhs_signed, UnsignedRHS rhs_unsigned) noexcept -> ModeBasedReturnType<R, return_mode>
{
    return Add<return_mode, R>(rhs_unsigned, lhs_signed);
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class FloatingLHS,
          class FloatingRHS,
          class R,
          typename std::enable_if_t<std::is_floating_point<FloatingLHS>::value &&
                                        std::is_floating_point<FloatingRHS>::value && std::is_floating_point<R>::value,
                                    bool>>
auto Add(FloatingLHS lhs_floating, FloatingRHS rhs_floating) noexcept -> ModeBasedReturnType<R, return_mode>
{
    static_assert(std::numeric_limits<FloatingLHS>::is_iec559 && std::numeric_limits<FloatingRHS>::is_iec559,
                  "Operands must adhere to IEEE 754 for ensured accuracy of results");

    return details::FloatingPointEnvironment::CalculateAndVerify<return_mode>(
        // Suppress "AUTOSAR C++14 A7-1-7", The rule states: "Each expression statement and identifier
        // declaration shall be placed on a separate line." The code here is present in single line to avoid
        // clang format failure.
        // coverity[autosar_cpp14_a7_1_7_violation]
        [lhs_floating, rhs_floating]() noexcept {
            return lhs_floating + rhs_floating;
        });
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class FloatingLHS,
          class IntegralRHS,
          class R,
          typename std::enable_if_t<std::is_floating_point<FloatingLHS>::value &&
                                        std::is_integral<IntegralRHS>::value && std::is_floating_point<R>::value,
                                    bool>>
// Suppress "UNCAUGHT_EXCEPT" rule findings. This rule states: "Called function throws an exception of type
// std::bad_variant_access".
// Rationale: There is no code path where the exception would be thrown.
// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// implicitly.".
// Rationale: There is no code path where std::terminate would be implicitly called.
// coverity[fun_call_w_exception]
// coverity[uncaught_exception]
// coverity[autosar_cpp14_a15_5_3_violation]
auto Add(FloatingLHS lhs_floating, IntegralRHS rhs_integral) noexcept -> ModeBasedReturnType<R, return_mode>
{
    const auto rhs_floating = Cast<return_mode, R>(rhs_integral);

    const auto add = [&lhs_floating](R rhs_casted) noexcept -> ModeBasedReturnType<R, return_mode> {
        return Add<return_mode, R>(lhs_floating, rhs_casted);
    };
    return PerformActionBasedOnReturnMode<return_mode, R>(add, rhs_floating);
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class IntegralLHS,
          class FloatingRHS,
          class R,
          typename std::enable_if_t<std::is_integral<IntegralLHS>::value &&
                                        std::is_floating_point<FloatingRHS>::value && std::is_floating_point<R>::value,
                                    bool>>
auto Add(IntegralLHS lhs_integral, FloatingRHS rhs_floating) noexcept -> ModeBasedReturnType<R, return_mode>
{
    return Add<return_mode, R>(rhs_floating, lhs_integral);
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class LHS,
          class SignedRHS,
          class R,
          typename std::enable_if_t<std::is_integral<LHS>::value && is_signed_integral<SignedRHS>::value &&
                                        std::is_integral<R>::value,
                                    bool>>
auto Subtract(LHS lhs, SignedRHS rhs_signed) noexcept -> ModeBasedReturnType<R, return_mode>
{
    if (CmpLess(rhs_signed, 0U))
    {
        // We actually add
        return Add<return_mode, R>(lhs, Abs(rhs_signed));
    }
    else
    {
        using UnsignedRHS = std::make_unsigned_t<SignedRHS>;
        const auto rhs_unsigned = static_cast<UnsignedRHS>(rhs_signed);
        return Subtract<return_mode, R>(lhs, rhs_unsigned);
    }
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class SignedLHS,
          class UnsignedRHS,
          class R,
          typename std::enable_if_t<is_signed_integral<SignedLHS>::value && is_unsigned_integral<UnsignedRHS>::value &&
                                        std::is_integral<R>::value,
                                    bool>>
// Suppress "UNCAUGHT_EXCEPT" rule findings. This rule states: "Called function throws an exception of type
// std::bad_variant_access".
// Rationale: There is no code path where the exception would be thrown.
// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// implicitly.".
// Rationale: There is no code path where std::terminate would be implicitly called.
// coverity[fun_call_w_exception]
// coverity[uncaught_exception]
// coverity[autosar_cpp14_a15_5_3_violation]
auto Subtract(SignedLHS lhs_signed, UnsignedRHS rhs_unsigned) noexcept -> ModeBasedReturnType<R, return_mode>
{
    if (CmpLess(lhs_signed, 0))
    {
        // Result will be negative

        // Inputs marked by {}: {-t} - {u} = -(t + u)
        const auto intermediate = Add<return_mode, std::uint64_t>(Abs(lhs_signed), rhs_unsigned);

        if constexpr (return_mode == ReturnMode::kReturnResultOnError)
        {
            if (!intermediate.has_value())
            {
                return score::MakeUnexpected<R>(intermediate.error());
            }
            return Negate<return_mode, R>(*intermediate);
        }
        else if constexpr (return_mode == ReturnMode::kAbortOnError)
        {
            return Negate<return_mode, R>(intermediate);
        }
        else
        {
            EmitCompilerDiagnosticForUnhandledEnumerator<return_mode>();
        }
    }
    else
    {
        using UnsignedLHS = std::make_unsigned_t<SignedLHS>;
        const auto lhs_unsigned = static_cast<UnsignedLHS>(lhs_signed);
        return Subtract<return_mode, R>(lhs_unsigned, rhs_unsigned);
    }
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class UnsignedLHS,
          class UnsignedRHS,
          class R,
          typename std::enable_if_t<is_unsigned_integral<UnsignedLHS>::value &&
                                        is_unsigned_integral<UnsignedRHS>::value && std::is_integral<R>::value,
                                    bool>>
// Suppress "UNCAUGHT_EXCEPT" rule findings. This rule states: "Called function throws an exception of type
// std::bad_variant_access".
// Rationale: There is no code path where the exception would be thrown.
// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// implicitly.".
// Rationale: There is no code path where std::terminate would be implicitly called.
// coverity[fun_call_w_exception]
// coverity[uncaught_exception]
// coverity[autosar_cpp14_a15_5_3_violation]
auto Subtract(UnsignedLHS lhs_unsigned, UnsignedRHS rhs_unsigned) noexcept -> ModeBasedReturnType<R, return_mode>
{
    if (CmpLess(lhs_unsigned, rhs_unsigned))
    {
        // Result will be negative
        // Inputs marked by {}: {lhs} - {rhs} = -(rhs - lhs)
        const auto lhs_unsigned_cast = static_cast<UnsignedRHS>(lhs_unsigned);
        const auto reverse_operation_result = static_cast<UnsignedRHS>(rhs_unsigned - lhs_unsigned_cast);
        return Negate<return_mode, R>(reverse_operation_result);
    }
    else
    {
        // Result will be positive
        using intermediary_type = bigger_type_t<UnsignedLHS, UnsignedRHS>;
        const auto result = static_cast<intermediary_type>(lhs_unsigned - rhs_unsigned);
        return Cast<return_mode, R>(result);
    }
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class FloatingLHS,
          class FloatingRHS,
          class R,
          typename std::enable_if_t<std::is_floating_point<FloatingLHS>::value &&
                                        std::is_floating_point<FloatingRHS>::value && std::is_floating_point<R>::value,
                                    bool>>
auto Subtract(FloatingLHS lhs_floating, FloatingRHS rhs_floating) noexcept -> ModeBasedReturnType<R, return_mode>
{
    static_assert(std::numeric_limits<FloatingLHS>::is_iec559 && std::numeric_limits<FloatingRHS>::is_iec559,
                  "Operands must adhere to IEEE 754 for ensured accuracy of results");

    return details::FloatingPointEnvironment::CalculateAndVerify<return_mode>(
        // coverity[autosar_cpp14_a7_1_7_violation]
        [lhs_floating, rhs_floating]() noexcept {
            return lhs_floating - rhs_floating;
        });
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class FloatingLHS,
          class IntegralRHS,
          class R,
          typename std::enable_if_t<std::is_floating_point<FloatingLHS>::value &&
                                        std::is_integral<IntegralRHS>::value && std::is_floating_point<R>::value,
                                    bool>>
// Suppress "UNCAUGHT_EXCEPT" rule findings. This rule states: "Called function throws an exception of type
// std::bad_variant_access".
// Rationale: There is no code path where the exception would be thrown.
// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// implicitly.".
// Rationale: There is no code path where std::terminate would be implicitly called.
// coverity[fun_call_w_exception]
// coverity[uncaught_exception]
// coverity[autosar_cpp14_a15_5_3_violation]
auto Subtract(FloatingLHS lhs_floating, IntegralRHS rhs_integral) noexcept -> ModeBasedReturnType<R, return_mode>
{
    const auto rhs_floating = Cast<return_mode, R>(rhs_integral);

    const auto subtract = [&lhs_floating](R rhs_casted) noexcept -> ModeBasedReturnType<R, return_mode> {
        return Subtract<return_mode, R>(lhs_floating, rhs_casted);
    };
    return PerformActionBasedOnReturnMode<return_mode, R>(subtract, rhs_floating);
}

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
template <ReturnMode return_mode,
          class TempR,
          class IntegralLHS,
          class FloatingRHS,
          class R,
          typename std::enable_if_t<std::is_integral<IntegralLHS>::value &&
                                        std::is_floating_point<FloatingRHS>::value && std::is_floating_point<R>::value,
                                    bool>>
// Suppress "UNCAUGHT_EXCEPT" rule findings. This rule states: "Called function throws an exception of type
// std::bad_variant_access".
// Rationale: There is no code path where the exception would be thrown.
// Suppress "AUTOSAR C++14 A15-5-3" rule findings. This rule states: "The std::terminate() function shall not be called
// implicitly.".
// Rationale: There is no code path where std::terminate would be implicitly called.
// coverity[fun_call_w_exception]
// coverity[uncaught_exception]
// coverity[autosar_cpp14_a15_5_3_violation]
auto Subtract(IntegralLHS lhs_integral, FloatingRHS rhs_floating) noexcept -> ModeBasedReturnType<R, return_mode>
{
    const auto lhs_floating = Cast<return_mode, R>(lhs_integral);

    const auto subtract = [&rhs_floating](R lhs_casted) noexcept -> ModeBasedReturnType<R, return_mode> {
        return Subtract<return_mode, R>(lhs_casted, rhs_floating);
    };
    return PerformActionBasedOnReturnMode<return_mode, R>(subtract, lhs_floating);
}

// Type-first convenience overloads: allow specifying only the return type (TempR) without explicitly restating the
// default ReturnMode.
template <class TempR, class LHS, class RHS>
auto Add(LHS lhs, RHS rhs) noexcept -> decltype(Add<kDefaultReturnMode, TempR>(lhs, rhs))
{
    return Add<kDefaultReturnMode, TempR>(lhs, rhs);
}

template <class TempR, class LHS, class RHS>
auto Subtract(LHS lhs, RHS rhs) noexcept -> decltype(Subtract<kDefaultReturnMode, TempR>(lhs, rhs))
{
    return Subtract<kDefaultReturnMode, TempR>(lhs, rhs);
}

}  // namespace score::safe_math

#endif  // SCORE_LIB_SAFE_MATH_DETAILS_ADDITION_SUBTRACTION_ADDITION_SUBTRACTION_H
