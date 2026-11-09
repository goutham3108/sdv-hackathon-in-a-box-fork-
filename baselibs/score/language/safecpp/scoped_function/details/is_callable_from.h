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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_IS_CALLABLE_FROM_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_IS_CALLABLE_FROM_H

#include <score/type_traits.hpp>

namespace score::safecpp::details
{

template <bool NoExcept, class FunctionType>
class IsInvocable;

template <class ReturnType, class... Args>
class IsInvocable<false, ReturnType(Args...)>
{
  public:
    template <class Callable>
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // Suppress "AUTOSAR C++14 A0-1-1" rule finding: "A project shall not contain instances of non-volatile variables
    // being given values that are not subsequently used."
    // Rationale: This variable is used below.
    // coverity[autosar_cpp14_a5_1_7_violation]
    // coverity[autosar_cpp14_a0_1_1_violation]
    static constexpr bool is_invocable_r_v{score::cpp::is_invocable_r<ReturnType, Callable, Args...>::value};
};

template <class ReturnType, class... Args>
class IsInvocable<true, ReturnType(Args...)>
{
  public:
    template <class Callable>
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    static constexpr bool is_invocable_r_v{std::is_nothrow_invocable_r_v<ReturnType, Callable, Args...>};
};

template <class FunctionType, class Callable>
class CallableTraits;

template <class ReturnType, class... Args, class Callable>
class CallableTraits<ReturnType(Args...), Callable> final : public IsInvocable<false, ReturnType(Args...)>
{
  public:
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using QualifiedCallable = Callable;
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using QualifiedCallableReference = Callable&;
};

template <class ReturnType, class... Args, class Callable>
class CallableTraits<ReturnType(Args...) const, Callable> final : public IsInvocable<false, ReturnType(Args...)>
{
  public:
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using QualifiedCallable = const Callable;
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using QualifiedCallableReference = const Callable&;
};

template <class ReturnType, class... Args, class Callable>
class CallableTraits<ReturnType(Args...) noexcept, Callable> final : public IsInvocable<true, ReturnType(Args...)>
{
  public:
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using QualifiedCallable = Callable;
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using QualifiedCallableReference = Callable&;
};

template <class ReturnType, class... Args, class Callable>
class CallableTraits<ReturnType(Args...) const noexcept, Callable> final : public IsInvocable<true, ReturnType(Args...)>
{
  public:
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using QualifiedCallable = const Callable;
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using QualifiedCallableReference = const Callable&;
};

template <class Callable, class FunctionType>
class IsCallableFrom
{
  private:
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using QualifiedCallable = typename CallableTraits<FunctionType, Callable>::QualifiedCallable;
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using QualifiedCallableReference = typename CallableTraits<FunctionType, Callable>::QualifiedCallableReference;

  public:
    // Suppress "AUTOSAR C++14 A0-1-1" rule finding. This rule states: "A project shall not contain instances of
    // non-volatile variables being given values that are not subsequently used.".
    // Rationale: The variable is a static member and is used in constexpr statements.
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a0_1_1_violation]
    // coverity[autosar_cpp14_a5_1_7_violation]
    static constexpr bool value{
        CallableTraits<FunctionType, Callable>::template is_invocable_r_v<QualifiedCallable> &&
        CallableTraits<FunctionType, Callable>::template is_invocable_r_v<QualifiedCallableReference>};
};

// Suppress "AUTOSAR C++14 A2-10-4" rule finding. This rule states: "The identifier name of a non-member object with
// static storage duration or static function shall not be reused within a namespace.".
// Rationale: The identifier name `IsCallableFromV` is unique within this namespace and is not reused across other
// sources.
// Suppress "AUTOSAR C++14 A0-1-1" rule finding. This rule states: "A project shall not contain instances of
// non-volatile variables being given values that are not subsequently used.".
// Rationale: The variable is used in the context of SFINAE.
template <class Callable, class FunctionType>
// Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
// Rationale: There is not decltype or typeid.
// coverity[autosar_cpp14_a5_1_7_violation]
// coverity[autosar_cpp14_a2_10_4_violation: FALSE]
// coverity[autosar_cpp14_a0_1_1_violation]
inline constexpr bool IsCallableFromV = IsCallableFrom<Callable, FunctionType>::value;

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_IS_CALLABLE_FROM_H
