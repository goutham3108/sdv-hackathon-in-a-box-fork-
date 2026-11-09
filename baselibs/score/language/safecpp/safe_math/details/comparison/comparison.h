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
#ifndef SCORE_LIB_SAFE_MATH_DETAILS_COMPARISON_COMPARISON_H
#define SCORE_LIB_SAFE_MATH_DETAILS_COMPARISON_COMPARISON_H

#include "score/language/safecpp/safe_math/details/type_traits/type_traits.h"
#include "score/language/safecpp/safe_math/error.h"
#include "score/result/result.h"

#include <score/math.hpp>

#include <limits>
#include <type_traits>

// Reimplementation of C++20 utility functions for safe comparison of integer values
// https://en.cppreference.com/w/cpp/utility/intcmp

namespace score::safe_math
{

// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// The call is required to circumvent an implementation specific bug and is guarded by an additional check so that it is
// only applied for the implementation that exhibits this bug in GCC.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__GNUC__) && (__GNUC__ == 8) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ == 4) && !defined(__clang__) && \
    !defined(__QNX__)
// The GCC optimization tree-forwprop performs forwarding of single use variables from their definition site into their
// use site. Here in this function we run into issues that some of the optimizations done by tree-frowprop might lead to
// overflows or underflows with given input parameters. GCC provides a warning with -Wstrict-overflow that informs about
// this situation. From the warning it is unknown whether GCC would apply said optimization in such cases or not. For
// safety reasons we disable the optimization for this function in GCC on host, as we only see the warnings there.
// TODO: Further investigation into this topic will be handled in ticket Ticket-79675

// The call is required to circumvent an implementation specific bug and is guarded by an additional check so that it is
// only applied for the implementation that exhibits this bug in GCC. GCC first sends push_options to enable setting a
// specific option and then using optimize one can set required configuration.
// Suppress "AUTOSAR C++14 A16-7-1", The rule states: "The #pragma directive shall not be used.
// coverity[autosar_cpp14_a16_7_1_violation]
// coverity[autosar_cpp14_a16_0_1_violation]
#pragma GCC push_options
// coverity[autosar_cpp14_a16_7_1_violation]
// coverity[autosar_cpp14_a16_0_1_violation]
#pragma GCC optimize("tree-forwprop")
// coverity[autosar_cpp14_a16_0_1_violation]
#endif

template <class Lhs,
          class Rhs,
          typename std::enable_if_t<(is_signed_integral<Lhs>::value && is_signed_integral<Rhs>::value) ||
                                        (is_unsigned_integral<Lhs>::value && is_unsigned_integral<Rhs>::value),
                                    bool> = true>
constexpr bool CmpLess(Lhs lhs, Rhs rhs) noexcept
{
    using BiggerType = bigger_type_t<Lhs, Rhs>;
    return static_cast<BiggerType>(lhs) < static_cast<BiggerType>(rhs);
}

template <class SignedLhs,
          class UnsignedRhs,
          typename std::enable_if_t<is_signed_integral<SignedLhs>::value && is_unsigned_integral<UnsignedRhs>::value,
                                    bool> = true>
constexpr bool CmpLess(SignedLhs lhs_signed, UnsignedRhs rhs_unsigned) noexcept
{
    // Suppress AUTOSAR C++14 A5-2-2 rule findings. This rule stated: "Traditional C-style casts shall not be used."
    // This is a false positive. SignedLhs{0} is using C++11 uniform initialization.
    // coverity[autosar_cpp14_a5_2_2_violation]
    if (CmpLess(lhs_signed, SignedLhs{0}))
    {
        return true;
    }
    using BiggerType = bigger_type_t<std::make_unsigned_t<SignedLhs>, UnsignedRhs>;
    return static_cast<BiggerType>(lhs_signed) < static_cast<BiggerType>(rhs_unsigned);
}

template <class UnsignedLhs,
          class SignedRhs,
          typename std::enable_if_t<is_unsigned_integral<UnsignedLhs>::value && is_signed_integral<SignedRhs>::value,
                                    bool> = true>
constexpr bool CmpLess(UnsignedLhs lhs_unsigned, SignedRhs signed_rhs) noexcept
{
    // coverity[autosar_cpp14_a5_2_2_violation]
    if (CmpLess(signed_rhs, SignedRhs{0}))
    {
        return false;
    }
    using BiggerUnsignedType = bigger_type_t<UnsignedLhs, std::make_unsigned_t<SignedRhs>>;
    return static_cast<BiggerUnsignedType>(lhs_unsigned) < static_cast<BiggerUnsignedType>(signed_rhs);
}

template <class FloatingLhs,
          class FloatingRhs,
          typename std::enable_if_t<(std::is_floating_point<FloatingLhs>::value &&
                                     std::is_floating_point<FloatingRhs>::value),
                                    bool> = true>
constexpr bool CmpLess(FloatingLhs lhs, FloatingRhs rhs) noexcept
{
    using BiggerType = bigger_type_t<FloatingLhs, FloatingRhs>;
    return static_cast<BiggerType>(lhs) < static_cast<BiggerType>(rhs);
}

// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__GNUC__) && (__GNUC__ == 8) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ == 4) && !defined(__clang__) && \
    !defined(__QNX__)
// This unsets the tree-forwprop  option and hence should not be removed.
// coverity[autosar_cpp14_a16_7_1_violation]
// coverity[autosar_cpp14_a16_0_1_violation]
#pragma GCC pop_options
// coverity[autosar_cpp14_a16_0_1_violation]
#endif

namespace details
{
template <class T, typename = std::enable_if_t<std::is_integral<T>::value>>
using Type64 = std::conditional_t<std::is_signed<T>::value, std::int64_t, std::uint64_t>;

template <class Floating, typename std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
constexpr bool IsAboveMaxUint64T(Floating value) noexcept
{
    static_assert(std::numeric_limits<double>::is_iec559 == true, "double doesn't conform to IEC559/IEEE754");
    static_assert(std::int64_t{-1} == static_cast<std::int64_t>(~(std::uint64_t{0U})),
                  "We expect 2's complement representation of std::uint64_t.");

    // Everything that is higher than this number will be mapped to 2^64 which is above the maximum of an uint64_t.
    constexpr double next_double_below_max_uint64_t{18446744073709549568.0};
    return CmpLess(next_double_below_max_uint64_t, value);
}

template <class Floating, typename std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
constexpr bool IsAboveMaxInt64T(Floating value) noexcept
{
    static_assert(std::numeric_limits<double>::is_iec559 == true, "double doesn't conform to IEC559/IEEE754");
    static_assert(std::int64_t{-1} == static_cast<std::int64_t>(~(std::uint64_t{0U})),
                  "We expect 2's complement representation of std::int64_t.");

    // Everything that is higher than this number will be mapped to 2^64 which is above the maximum of an uint64_t.
    constexpr double next_double_below_max_int64_t{9223372036854774784.0};
    return (!score::cpp::signbit(value) && score::cpp::isinf(value)) || CmpLess(next_double_below_max_int64_t, value);
}

template <class Floating, typename std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
constexpr bool IsBelowMinInt64T(Floating value) noexcept
{
    static_assert(std::numeric_limits<double>::is_iec559 == true, "double doesn't conform to IEC559/IEEE754");
    static_assert(std::int64_t{-1} == static_cast<std::int64_t>(~(std::uint64_t{0U})),
                  "We expect 2's complement representation of std::int64_t.");

    // Everything that is higher than this number will be mapped to 2^64 which is above the maximum of an uint64_t.
    constexpr double next_double_above_min_int64_t{-9223372036854775808.0};
    return CmpLess(value, next_double_above_min_int64_t);
}

enum class RangeCheckResult : std::uint8_t
{
    kBelow = 0U,
    kInRange,
    kAbove,
};

template <class Integral,
          class Floating,
          typename std::enable_if_t<std::is_integral<Integral>::value && std::is_floating_point<Floating>::value,
                                    bool> = true>
RangeCheckResult IsInRange(Floating value) noexcept
{
    // Suppress "AUTOSAR C++14 M0-1-2" rule finding.
    // This rule states: "A project shall not contain infeasible paths."
    // Suppress "AUTOSAR C++14 M0-1-9" rule finding.
    // This rule states: "There shall be no dead code."
    // Rational for both: This is not dead code but part compile-time evaluated. Separating the evaluation, makes the
    // check more complex and therefore error-prone.
    // coverity[autosar_cpp14_m0_1_2_violation]
    // coverity[autosar_cpp14_m0_1_9_violation]
    if ((std::is_unsigned<Integral>::value && details::IsAboveMaxUint64T(value)) ||
        (std::is_signed<Integral>::value && details::IsAboveMaxInt64T(value)))
    {
        return RangeCheckResult::kAbove;
    }

    // Suppress "AUTOSAR C++14 M0-1-2" rule finding.
    // This rule states: "A project shall not contain infeasible paths."
    // Suppress "AUTOSAR C++14 M0-1-9" rule finding.
    // This rule states: "There shall be no dead code."
    // Rational for both: This is not dead code but part compile-time evaluated. Separating the evaluation, makes the
    // check more complex and therefore error-prone.
    // coverity[autosar_cpp14_m0_1_2_violation]
    // coverity[autosar_cpp14_m0_1_9_violation]
    if ((std::is_unsigned<Integral>::value && CmpLess(value, 0.0)) ||
        (std::is_signed<Integral>::value && details::IsBelowMinInt64T(value)))
    {
        return RangeCheckResult::kBelow;
    }

    return RangeCheckResult::kInRange;
}

}  // namespace details

template <class FloatingLhs,
          class IntegralRhs,
          typename std::enable_if_t<std::is_floating_point<FloatingLhs>::value && std::is_integral<IntegralRhs>::value,
                                    bool> = true>
constexpr bool CmpLess(FloatingLhs lhs_floating, IntegralRhs rhs_integral) noexcept
{
    if (score::cpp::isnan(lhs_floating))
    {
        return false;
    }

    // Return false if +inf and true if -inf
    if (score::cpp::isinf(lhs_floating))
    {
        return score::cpp::signbit(lhs_floating);
    }

    // Suppress "AUTOSAR C++14 M6-4-3" rule finding.
    // This rule states: "A switch statement shall be a well-formed switch statement."
    // Rationale: This is a well-formed switch statement that uses returns to terminate every non-empty switch-clause
    // Suppress "AUTOSAR C++14 M6-4-5" rule finding.
    // This rule states: "An unconditional throw or break statement shall terminate every non-empty switch-clause."
    // Rationale: This is a well-formed switch statement that uses returns to terminate every non-empty switch-clause
    // coverity[autosar_cpp14_m6_4_3_violation]
    switch (details::IsInRange<IntegralRhs>(lhs_floating))
    {
        // coverity[autosar_cpp14_m6_4_5_violation]
        case details::RangeCheckResult::kBelow:
            return true;
        // coverity[autosar_cpp14_m6_4_5_violation]
        case details::RangeCheckResult::kAbove:
            return false;
        // coverity[autosar_cpp14_m6_4_5_violation]
        case details::RangeCheckResult::kInRange:
            [[fallthrough]];
        // coverity[autosar_cpp14_m6_4_5_violation]
        default:
        {
            auto lhs_integral = static_cast<details::Type64<IntegralRhs>>(score::cpp::floor(lhs_floating));
            return CmpLess(lhs_integral, rhs_integral);
        }
    }
}

template <class IntegralLhs,
          class FloatingRhs,
          typename std::enable_if_t<std::is_integral<IntegralLhs>::value && std::is_floating_point<FloatingRhs>::value,
                                    bool> = true>
constexpr bool CmpLess(IntegralLhs lhs_integral, FloatingRhs rhs_floating) noexcept
{
    if (score::cpp::isnan(rhs_floating))
    {
        return false;
    }

    // Return true if +inf and false if -inf
    if (score::cpp::isinf(rhs_floating))
    {
        return !score::cpp::signbit(rhs_floating);
    }

    // Suppress "AUTOSAR C++14 M6-4-3" rule finding.
    // This rule states: "A switch statement shall be a well-formed switch statement."
    // Rationale: This is a well-formed switch statement that uses returns to terminate every non-empty switch-clause
    // Suppress "AUTOSAR C++14 M6-4-5" rule finding.
    // This rule states: "An unconditional throw or break statement shall terminate every non-empty switch-clause."
    // Rationale: This is a well-formed switch statement that uses returns to terminate every non-empty switch-clause
    // coverity[autosar_cpp14_m6_4_3_violation]
    switch (details::IsInRange<IntegralLhs>(rhs_floating))
    {
        // coverity[autosar_cpp14_m6_4_5_violation]
        case details::RangeCheckResult::kBelow:
            return false;
        // coverity[autosar_cpp14_m6_4_5_violation]
        case details::RangeCheckResult::kAbove:
            return true;
        // coverity[autosar_cpp14_m6_4_5_violation]
        case details::RangeCheckResult::kInRange:
            [[fallthrough]];
        // coverity[autosar_cpp14_m6_4_5_violation]
        default:
        {
            auto rhs_integral = static_cast<details::Type64<IntegralLhs>>(score::cpp::ceil(rhs_floating));
            return CmpLess(lhs_integral, rhs_integral);
        }
    }
}

// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__GNUC__) && (__GNUC__ == 8) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ == 4) && !defined(__clang__) && \
    !defined(__QNX__)
// The GCC optimization tree-forwprop performs forwarding of single use variables from their definition site into their
// use site. Here in this function we run into issues that some of the optimizations done by tree-frowprop might lead to
// overflows or underflows with given input parameters. GCC provides a warning with -Wstrict-overflow that informs about
// this situation. From the warning it is unknown whether GCC would apply said optimization in such cases or not. For
// safety reasons we disable the optimization for this function in GCC on host, as we only see the warnings there.
// TODO: Further investigation into this topic will be handled in ticket Ticket-79675

// coverity[autosar_cpp14_a16_7_1_violation]
// coverity[autosar_cpp14_a16_0_1_violation]
#pragma GCC push_options
// coverity[autosar_cpp14_a16_7_1_violation]
// coverity[autosar_cpp14_a16_0_1_violation]
#pragma GCC optimize("tree-forwprop")
// coverity[autosar_cpp14_a16_0_1_violation]
#endif

template <class Lhs,
          class Rhs,
          typename std::enable_if_t<(is_signed_integral<Lhs>::value && is_signed_integral<Rhs>::value) ||
                                        (is_unsigned_integral<Lhs>::value && is_unsigned_integral<Rhs>::value),
                                    bool> = true>
constexpr bool CmpEqual(Lhs lhs, Rhs rhs) noexcept
{
    using BiggerType = bigger_type_t<Lhs, Rhs>;
    return static_cast<BiggerType>(lhs) == static_cast<BiggerType>(rhs);
}

template <class SignedLhs,
          class UnsignedRhs,
          typename std::enable_if_t<is_signed_integral<SignedLhs>::value && is_unsigned_integral<UnsignedRhs>::value,
                                    bool> = true>
constexpr bool CmpEqual(SignedLhs lhs_signed, UnsignedRhs rhs_unsigned) noexcept
{
    using BiggerUnsignedType = bigger_type_t<std::make_unsigned_t<SignedLhs>, UnsignedRhs>;
    // Suppress AUTOSAR C++14 A5-2-2 rule findings. This rule states: "Traditional C-style casts shall not be used."
    // Rationale: This is not a cast but an instantiation of a specific type.
    // coverity[autosar_cpp14_a5_2_2_violation]
    if (CmpLess(lhs_signed, SignedLhs{0}))
    {
        return false;
    }
    return static_cast<BiggerUnsignedType>(lhs_signed) == static_cast<BiggerUnsignedType>(rhs_unsigned);
}

// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__GNUC__) && (__GNUC__ == 8) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ == 4) && !defined(__clang__) && \
    !defined(__QNX__)
// coverity[autosar_cpp14_a16_7_1_violation]
// coverity[autosar_cpp14_a16_0_1_violation]
#pragma GCC pop_options
// coverity[autosar_cpp14_a16_0_1_violation]
#endif

template <class UnsignedLhs,
          class SignedRhs,
          typename std::enable_if_t<is_unsigned_integral<UnsignedLhs>::value && is_signed_integral<SignedRhs>::value,
                                    bool> = true>
constexpr bool CmpEqual(UnsignedLhs lhs_unsigned, SignedRhs rhs_signed) noexcept
{
    return CmpEqual(rhs_signed, lhs_unsigned);
}

// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__GNUC__) && (__GNUC__ == 8) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ == 4) && !defined(__clang__) && \
    !defined(__QNX__)
// The GCC optimization tree-forwprop performs forwarding of single use variables from their definition site into their
// use site. Here in this function we run into issues that some of the optimizations done by tree-frowprop might lead to
// overflows or underflows with given input parameters. GCC provides a warning with -Wstrict-overflow that informs about
// this situation. From the warning it is unknown whether GCC would apply said optimization in such cases or not. For
// safety reasons we disable the optimization for this function in GCC on host, as we only see the warnings there.
// TODO: Further investigation into this topic will be handled in ticket Ticket-79675

// coverity[autosar_cpp14_a16_7_1_violation]
// coverity[autosar_cpp14_a16_0_1_violation]
#pragma GCC push_options
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_a16_7_1_violation]
#pragma GCC optimize("tree-forwprop")
// coverity[autosar_cpp14_a16_0_1_violation]
#endif

template <class FloatingLhs,
          class FloatingRhs,
          typename std::enable_if_t<(std::is_floating_point<FloatingLhs>::value &&
                                     std::is_floating_point<FloatingRhs>::value),
                                    bool> = true>
constexpr bool CmpEqual(FloatingLhs lhs, FloatingRhs rhs, std::int32_t tolerance) noexcept
{
    // Whenever we compare with NaN the result must be `false` (IEEE 754)
    if (score::cpp::isnan(lhs) || score::cpp::isnan(rhs))
    {
        return false;
    }

    // Comparison based on ULPs (units in the last place). For this we step `ulps` times from lhs towards rhs. This
    // gives us a window [lhs;ulp_bound] or respectively [ulp_bound;lhs] where we can later on check whether rhs is
    // within of that window or not.

    using BiggerType = bigger_type_t<FloatingLhs, FloatingRhs>;
    BiggerType lhs_big{lhs};
    BiggerType rhs_big{rhs};
    BiggerType ulp_bound{lhs_big};
    for (auto i = 0; i < tolerance; ++i)
    {
        ulp_bound = score::cpp::nexttoward(ulp_bound, rhs_big);
    }

    return ((lhs_big >= rhs_big) && (rhs_big >= ulp_bound)) || ((lhs_big <= rhs_big) && (rhs_big <= ulp_bound));
}

// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__GNUC__) && (__GNUC__ == 8) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ == 4) && !defined(__clang__) && \
    !defined(__QNX__)
// coverity[autosar_cpp14_a16_7_1_violation]
// coverity[autosar_cpp14_a16_0_1_violation]
#pragma GCC pop_options
// coverity[autosar_cpp14_a16_0_1_violation]
#endif

template <
    class FloatingLhs,
    class IntegralRhs,
    typename std::enable_if_t<(std::is_floating_point<FloatingLhs>::value && std::is_integral<IntegralRhs>::value),
                              bool> = true>
constexpr bool CmpEqual(FloatingLhs lhs_floating, IntegralRhs rhs_integral, std::int32_t tolerance) noexcept
{
    // Whenever we compare with NaN the result must be `false` (IEEE 754)
    if (score::cpp::isnan(lhs_floating))
    {
        return false;
    }

    // Comparison based on ULPs (units in the last place). For this we step `ulps` times from lhs in both directions.
    // This gives us a window [lower_bound;upper_bound] where we can later on check whether rhs_integral is
    // inside that window or not.
    FloatingLhs upper_bound{lhs_floating};
    FloatingLhs lower_bound{lhs_floating};
    for (auto i = 0; i < tolerance; ++i)
    {
        upper_bound = score::cpp::nexttoward(upper_bound, std::numeric_limits<FloatingLhs>::max());
        lower_bound = score::cpp::nexttoward(lower_bound, std::numeric_limits<FloatingLhs>::lowest());
    }

    return !(CmpLess(rhs_integral, lower_bound) || CmpLess(upper_bound, rhs_integral));
}

template <
    class IntegralLhs,
    class FloatingRhs,
    typename std::enable_if_t<(std::is_integral<IntegralLhs>::value && std::is_floating_point<FloatingRhs>::value),
                              bool> = true>
constexpr bool CmpEqual(IntegralLhs lhs_integral, FloatingRhs rhs_floating, std::int32_t tolerance) noexcept
{
    return CmpEqual(rhs_floating, lhs_integral, tolerance);
}

template <class Lhs,
          class Rhs,
          typename std::enable_if_t<std::is_integral<Lhs>::value && std::is_integral<Rhs>::value, bool> = true>
constexpr bool CmpNotEqual(Lhs lhs, Rhs rhs) noexcept
{
    return !CmpEqual(lhs, rhs);
}

template <class Lhs,
          class Rhs,
          typename std::enable_if_t<(std::is_floating_point<Lhs>::value && std::is_floating_point<Rhs>::value) ||
                                        (std::is_floating_point<Lhs>::value && std::is_integral<Rhs>::value) ||
                                        (std::is_integral<Lhs>::value && std::is_floating_point<Rhs>::value),
                                    bool> = true>
constexpr bool CmpNotEqual(Lhs lhs, Rhs rhs, std::int32_t tolerance) noexcept
{
    return !CmpEqual(lhs, rhs, tolerance);
}

template <class Lhs, class Rhs>
constexpr bool CmpGreater(Lhs lhs, Rhs rhs) noexcept
{
    return CmpLess(rhs, lhs);
}

template <class Lhs,
          class Rhs,
          typename std::enable_if_t<std::is_integral<Lhs>::value && std::is_integral<Rhs>::value, bool> = true>
constexpr bool CmpLessEqual(Lhs lhs, Rhs rhs) noexcept
{
    return !CmpGreater(lhs, rhs);
}

template <class Lhs,
          class Rhs,
          typename std::enable_if_t<(std::is_floating_point<Lhs>::value && std::is_floating_point<Rhs>::value) ||
                                        (std::is_floating_point<Lhs>::value && std::is_integral<Rhs>::value) ||
                                        (std::is_integral<Lhs>::value && std::is_floating_point<Rhs>::value),
                                    bool> = true>
constexpr bool CmpLessEqual(Lhs lhs, Rhs rhs, std::int32_t tolerance) noexcept
{
    return CmpLess(lhs, rhs) || CmpEqual(lhs, rhs, tolerance);
}

template <class Lhs,
          class Rhs,
          typename std::enable_if_t<std::is_integral<Lhs>::value && std::is_integral<Rhs>::value, bool> = true>
constexpr bool CmpGreaterEqual(Lhs lhs, Rhs rhs) noexcept
{
    return !CmpLess(lhs, rhs);
}

template <class Lhs,
          class Rhs,
          typename std::enable_if_t<(std::is_floating_point<Lhs>::value && std::is_floating_point<Rhs>::value) ||
                                        (std::is_floating_point<Lhs>::value && std::is_integral<Rhs>::value) ||
                                        (std::is_integral<Lhs>::value && std::is_floating_point<Rhs>::value),
                                    bool> = true>
constexpr bool CmpGreaterEqual(Lhs lhs, Rhs rhs, std::int32_t tolerance) noexcept
{
    return CmpGreater(lhs, rhs) || CmpEqual(lhs, rhs, tolerance);
}

}  // namespace score::safe_math

#endif  // SCORE_LIB_SAFE_MATH_DETAILS_COMPARISON_COMPARISON_H
