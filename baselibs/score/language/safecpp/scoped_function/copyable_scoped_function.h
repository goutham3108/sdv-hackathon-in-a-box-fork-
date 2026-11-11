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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_COPYABLE_SCOPED_FUNCTION_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_COPYABLE_SCOPED_FUNCTION_H

#include "score/language/safecpp/scoped_function/details/allocator_aware_type_erasure_pointer.h"
#include "score/language/safecpp/scoped_function/details/function_wrapper.h"
#include "score/language/safecpp/scoped_function/details/invoker.h"
#include "score/language/safecpp/scoped_function/details/is_callable_from.h"
#include "score/language/safecpp/scoped_function/details/modify_signature.h"
#include "score/language/safecpp/scoped_function/details/scope_state.h"
#include "score/language/safecpp/scoped_function/details/scoped_function_invoker.h"
#include "score/language/safecpp/scoped_function/scope.h"

#include <score/stop_token.hpp>
#include <score/type_traits.hpp>

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

namespace score::safecpp
{

template <class FunctionType, class Allocator = std::allocator<std::uint8_t>>
class CopyableScopedFunction : public details::Invoker<details::ScopedFunctionInvoker,
                                                       details::CallOperatorInterface,
                                                       CopyableScopedFunction<FunctionType, Allocator>,
                                                       details::ModifySignatureT<FunctionType>>
{
  private:
    using BaseClass = details::Invoker<details::ScopedFunctionInvoker,
                                       details::CallOperatorInterface,
                                       CopyableScopedFunction<FunctionType, Allocator>,
                                       details::ModifySignatureT<FunctionType>>;

    template <class Callable>
    using FunctionWrapperImpl =
        typename details::FunctionWrapper<FunctionType>::template Implementation<score::cpp::remove_cvref_t<Callable>>;

    // Suppress "AUTOSAR C++14 A5-1-7" rule finding.
    // This rule states: "A lambda shall not be an operand to decltype or typeid.".
    // Suppress "AUTOSAR C++14 A0-1-1" rule finding. This rule states: "A project shall not contain instances of
    // non-volatile variables being given values that are not subsequently used.".
    template <class Callable>
    // coverity[autosar_cpp14_a5_1_7_violation: FALSE]: Neither decltype nor typeid is used here.
    // coverity[autosar_cpp14_a0_1_1_violation: FALSE]: The variable is a static member and is used.
    static constexpr bool IsViableCallable{
        std::conjunction_v<std::negation<std::is_same<score::cpp::remove_cvref_t<Callable>, CopyableScopedFunction>>,
                           details::IsCallableFrom<std::decay_t<Callable>, FunctionType>,
                           std::is_constructible<std::decay_t<Callable>, Callable>,
                           std::is_copy_constructible<std::decay_t<Callable>>>};

    CopyableScopedFunction(
        const Allocator& allocator,
        std::shared_ptr<details::ScopeState> scope_state,
        details::AllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>, Allocator> callable) noexcept
        : BaseClass{}, allocator_{allocator}, scope_state_{std::move(scope_state)}, callable_{callable}
    {
    }

  public:
    using ReturnType = typename BaseClass::ReturnType;

    // We explicitly do not provide the type trait allocator_type, since it would not be typed correctly because the
    // actual type of the allocated callable is unknown to us in the class scope. (type erasure)

    CopyableScopedFunction() noexcept : CopyableScopedFunction{Allocator{}} {}
    ~CopyableScopedFunction() noexcept override = default;

    explicit CopyableScopedFunction(const Allocator& allocator) noexcept
        : CopyableScopedFunction{
              allocator,
              nullptr,
              details::AllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>, Allocator>{allocator}}
    {
    }

    template <class Callable, class ScopeAllocator, class = std::enable_if_t<IsViableCallable<Callable>>>
    CopyableScopedFunction(const Scope<ScopeAllocator>& scope, Callable&& callable) noexcept
        : CopyableScopedFunction{std::allocator_arg, Allocator{}, scope, std::forward<Callable>(callable)}
    {
    }

    template <class Callable, class ScopeAllocator, class = std::enable_if_t<IsViableCallable<Callable>>>
    CopyableScopedFunction(std::allocator_arg_t,
                           const Allocator& allocator,
                           const Scope<ScopeAllocator>& scope,
                           Callable&& callable) noexcept
        : CopyableScopedFunction{
              allocator,
              scope.GetScopeState(),
              details::MakeAllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>,
                                                            FunctionWrapperImpl<Callable>,
                                                            Allocator>(allocator, std::forward<Callable>(callable))}
    {
    }

    // @note Be careful when using a ScopedFunction with a stop_token. This can have serious side effects.
    //       Before you proceed, you should carefully read the README.md.
    template <class Callable, class = std::enable_if_t<IsViableCallable<Callable>>>
    CopyableScopedFunction(const score::cpp::stop_token& stop_token, Callable&& callable) noexcept
        : CopyableScopedFunction{std::allocator_arg, Allocator{}, stop_token, std::forward<Callable>(callable)}
    {
    }

    // @note Be careful when using a ScopedFunction with a stop_token. This can have serious side effects.
    //       Before you proceed, you should carefully read the README.md.
    template <class Callable, class = std::enable_if_t<IsViableCallable<Callable>>>
    CopyableScopedFunction(std::allocator_arg_t,
                           const Allocator& allocator,
                           const score::cpp::stop_token& stop_token,
                           Callable&& callable) noexcept
        : CopyableScopedFunction{
              allocator,
              std::allocate_shared<details::ScopeState>(allocator, stop_token),
              details::MakeAllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>,
                                                            FunctionWrapperImpl<Callable>,
                                                            Allocator>(allocator, std::forward<Callable>(callable))}
    {
    }

    CopyableScopedFunction(const CopyableScopedFunction& other) = default;

    CopyableScopedFunction(const CopyableScopedFunction& other, const Allocator& allocator)
        : CopyableScopedFunction{allocator, other.scope_state_, {std::allocator_arg, allocator, other.callable_}}
    {
    }

    CopyableScopedFunction& operator=(const CopyableScopedFunction& other) = default;

    CopyableScopedFunction(CopyableScopedFunction&& other) noexcept = default;

    CopyableScopedFunction(CopyableScopedFunction&& other, const Allocator& allocator)
        : CopyableScopedFunction{allocator,
                                 std::move(other.scope_state_),
                                 {std::allocator_arg, allocator, std::move(other.callable_)}}
    {
    }

    CopyableScopedFunction& operator=(CopyableScopedFunction&& other) noexcept = default;

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return callable_ != nullptr;
    }

    [[nodiscard]] Allocator get_allocator() const noexcept
    {
        return callable_.get_allocator();
    }

  private:
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Friendship is the best way to gracefully support all variants of the function call operator correctly
    template <bool, class>
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class details::ScopedFunctionInvoker;

    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // To allow transferring the state from a CopyableScopedFunction to a MoveOnlyScopedFunction
    template <class, class>
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class MoveOnlyScopedFunction;

    details::AllocatorWrapper<Allocator> allocator_;
    std::shared_ptr<details::ScopeState> scope_state_;
    details::AllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>, Allocator> callable_;
};

}  // namespace score::safecpp

namespace std
{

// Suppress "AUTOSAR C++14 A11-0-2" rule finding. This rule states: "A type defined as struct shall: (1) provide only
// public data members, (2) not provide any special member functions or methods, (3) not be a base of another struct or
// class, (4) not inherit from another struct or class.".
// Rationale: Violates item 4 of this rule, but the standard requires this to be a struct.
// Suppress "AUTOSAR C++14 A17-6-1" rule finding. This rule states: "Non-standard entities shall not be added to
// standard namespaces.".
// Rationale: This is an allowed way to specify that a user type is allocator-aware.
template <class FunctionType, class Alloc>
// coverity[autosar_cpp14_a11_0_2_violation]
// coverity[autosar_cpp14_a17_6_1_violation]
struct uses_allocator<score::safecpp::CopyableScopedFunction<FunctionType, Alloc>, Alloc> : std::true_type
{
};

}  // namespace std

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_COPYABLE_SCOPED_FUNCTION_H
