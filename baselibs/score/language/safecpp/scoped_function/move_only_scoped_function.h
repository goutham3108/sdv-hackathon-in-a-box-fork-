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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_MOVE_ONLY_SCOPED_FUNCTION_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_MOVE_ONLY_SCOPED_FUNCTION_H

#include "score/language/safecpp/scoped_function/copyable_scoped_function.h"
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

#include <memory>
#include <type_traits>

namespace score::safecpp
{

template <class FunctionType, class Allocator = std::allocator<std::uint8_t>>
class MoveOnlyScopedFunction : public details::Invoker<details::ScopedFunctionInvoker,
                                                       details::CallOperatorInterface,
                                                       MoveOnlyScopedFunction<FunctionType, Allocator>,
                                                       details::ModifySignatureT<FunctionType>>
{
  private:
    using BaseClass = details::Invoker<details::ScopedFunctionInvoker,
                                       details::CallOperatorInterface,
                                       MoveOnlyScopedFunction<FunctionType, Allocator>,
                                       details::ModifySignatureT<FunctionType>>;

    template <class Callable>
    using FunctionWrapperImpl =
        typename details::FunctionWrapper<FunctionType>::template Implementation<score::cpp::remove_cvref_t<Callable>>;

    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // Suppress "AUTOSAR C++14 A0-1-1" rule finding. This rule states: "A project shall not contain instances of
    // non-volatile variables being given values that are not subsequently used.".
    template <class Callable>
    // coverity[autosar_cpp14_a5_1_7_violation]
    // coverity[autosar_cpp14_a0_1_1_violation: FALSE]: The var is a static member and is used in constexpr statements.
    static constexpr bool IsViableCallable{
        std::conjunction_v<std::negation<std::is_same<score::cpp::remove_cvref_t<Callable>, MoveOnlyScopedFunction>>,
                           details::IsCallableFrom<std::decay_t<Callable>, FunctionType>,
                           std::is_constructible<typename std::decay_t<Callable>, Callable>,
                           std::disjunction<std::is_move_constructible<typename std::decay_t<Callable>>,
                                            std::is_copy_constructible<typename std::decay_t<Callable>>>>};

    template <typename Callable>
    explicit MoveOnlyScopedFunction(const Allocator& allocator,
                                    const std::shared_ptr<details::ScopeState>& scope_state,
                                    Callable&& callable) noexcept
        : BaseClass{}, allocator_{allocator}, scope_state_{scope_state}, callable_{std::forward<Callable>(callable)}
    {
    }

  public:
    using ReturnType = typename BaseClass::ReturnType;

    // We explicitly do not provide the type trait allocator_type, since it would not be typed correctly because the
    // actual type of callable_ is unknown to us in the class scope. (type erasure)

    // Suppress "AUTOSAR C++14 A13-3-1" rule finding. This rule states:"A function that contains "forwarding reference"
    // as its argument shall not be overloaded.".
    // Rationale: The overload is not ambiguous, this is default constructor.
    // coverity[autosar_cpp14_a13_3_1_violation]
    MoveOnlyScopedFunction() noexcept : MoveOnlyScopedFunction{Allocator{}} {}
    ~MoveOnlyScopedFunction() noexcept override = default;

    // Suppress "AUTOSAR C++14 A13-3-1" rule finding. This rule states:"A function that contains "forwarding reference"
    // as its argument shall not be overloaded.".
    // Rationale: The overload is not ambiguous due to the different number and type of arguments.
    // coverity[autosar_cpp14_a13_3_1_violation]
    explicit MoveOnlyScopedFunction(const Allocator& allocator) noexcept
        : MoveOnlyScopedFunction{allocator, nullptr, allocator}
    {
    }

    template <class Callable, class ScopeAllocator, typename = std::enable_if_t<IsViableCallable<Callable>>>
    MoveOnlyScopedFunction(const Scope<ScopeAllocator>& scope, Callable&& callable) noexcept
        : MoveOnlyScopedFunction(std::allocator_arg, Allocator{}, scope, std::forward<Callable>(callable))
    {
    }

    template <class Callable, class ScopeAllocator, typename = std::enable_if_t<IsViableCallable<Callable>>>
    MoveOnlyScopedFunction(std::allocator_arg_t,
                           const Allocator& allocator,
                           const Scope<ScopeAllocator>& scope,
                           Callable&& callable) noexcept
        : MoveOnlyScopedFunction{
              allocator,
              scope.GetScopeState(),
              details::MakeAllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>,
                                                            FunctionWrapperImpl<Callable>,
                                                            Allocator>(allocator, std::forward<Callable>(callable))}
    {
    }

    // @note Be careful when using a ScopedFunction with a stop_token. This can have serious side effects.
    //       Before you proceed, you should carefully read the README.md.
    template <class Callable, typename = std::enable_if_t<IsViableCallable<Callable>>>
    MoveOnlyScopedFunction(const score::cpp::stop_token& stop_token, Callable&& callable) noexcept
        : MoveOnlyScopedFunction(std::allocator_arg, Allocator{}, stop_token, std::forward<Callable>(callable))
    {
    }

    // @note Be careful when using a ScopedFunction with a stop_token. This can have serious side effects.
    //       Before you proceed, you should carefully read the README.md.
    template <class Callable, typename = std::enable_if_t<IsViableCallable<Callable>>>
    MoveOnlyScopedFunction(std::allocator_arg_t,
                           const Allocator& allocator,
                           const score::cpp::stop_token& stop_token,
                           Callable&& callable) noexcept
        : MoveOnlyScopedFunction{
              allocator,
              std::allocate_shared<details::ScopeState>(allocator, stop_token),
              details::MakeAllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>,
                                                            FunctionWrapperImpl<Callable>,
                                                            Allocator>(allocator, std::forward<Callable>(callable))}
    {
    }

    MoveOnlyScopedFunction(const MoveOnlyScopedFunction& other) = delete;
    MoveOnlyScopedFunction& operator=(const MoveOnlyScopedFunction& other) = delete;
    // Suppress "AUTOSAR C++14 A13-3-1" rule finding. This rule states:"A function that contains "forwarding reference"
    // as its argument shall not be overloaded.".
    // Rationale: The overload is not ambiguous due to the different number and type of arguments.
    // coverity[autosar_cpp14_a13_3_1_violation]
    MoveOnlyScopedFunction(MoveOnlyScopedFunction&& other) noexcept = default;
    // Suppress "AUTOSAR C++14 A13-3-1" rule finding. This rule states:"A function that contains "forwarding reference"
    // as its argument shall not be overloaded.".
    // Rationale: The overload is not ambiguous due to the different number and type of arguments.
    // coverity[autosar_cpp14_a13_3_1_violation]
    MoveOnlyScopedFunction(MoveOnlyScopedFunction&& other, const Allocator& allocator)
        : MoveOnlyScopedFunction{
              allocator,
              std::move(other.scope_state_),
              details::AllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>, Allocator>{
                  std::allocator_arg,
                  allocator,
                  std::move(other.callable_)}}
    {
    }

    MoveOnlyScopedFunction& operator=(MoveOnlyScopedFunction&& other) noexcept = default;

    // Suppress "AUTOSAR C++14 A13-3-1" rule finding. This rule states:"A function that contains "forwarding reference"
    // as its argument shall not be overloaded.".
    // Rationale: The overload is not ambiguous due to the different number and type of arguments.

    // Must use the casting constructor that takes the allocator first and the copyable scoped function second, because
    // it must first extract the allocator from other and then move other. Doing it in reverse would be UB.
    template <class OtherAllocator>
    // coverity[autosar_cpp14_a13_3_1_violation]
    explicit MoveOnlyScopedFunction(CopyableScopedFunction<FunctionType, OtherAllocator>&& other) noexcept
        : MoveOnlyScopedFunction{
              std::allocator_arg,
              typename std::allocator_traits<OtherAllocator>::template rebind_alloc<Allocator>{other.get_allocator()},
              std::move(other)}
    {
    }

    // Suppress "AUTOSAR C++14 A13-3-1" rule finding. This rule states:"A function that contains "forwarding reference"
    // as its argument shall not be overloaded.".
    // Rationale: The overload is not ambiguous due to the different number and type of arguments.
    template <class OtherAllocator>
    // coverity[autosar_cpp14_a13_3_1_violation]
    MoveOnlyScopedFunction(CopyableScopedFunction<FunctionType, OtherAllocator>&& other,
                           const Allocator& allocator) noexcept
        : MoveOnlyScopedFunction{
              allocator,
              other.scope_state_,
              details::AllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>, Allocator>{
                  std::allocator_arg,
                  allocator,
                  std::move(other.callable_)}}
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return callable_ != nullptr;
    }

    [[nodiscard]] Allocator get_allocator() const noexcept
    {
        return callable_.get_allocator();
    }

  private:
    // Suppress "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Friendship is the best way to gracefully support all variants of the function call operator correctly
    template <bool, class>
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class details::ScopedFunctionInvoker;

    // Suppress "AUTOSAR C++14 A13-3-1" rule finding. This rule states:"A function that contains "forwarding reference"
    // as its argument shall not be overloaded.".
    // Rationale: The overload is not ambiguous due to the different number and type of arguments.

    // Required to support the casting constructor without allocator, that needs to first extract the allocator from
    // other and then move other. Doing it in reverse would be UB. We provide the other version in the public API, since
    // this keeps the API of CopyableScopedFunction and MoveOnlyScopedFunction symmetric.
    template <class OtherAllocator>
    // coverity[autosar_cpp14_a13_3_1_violation]
    MoveOnlyScopedFunction(std::allocator_arg_t,
                           const Allocator& allocator,
                           CopyableScopedFunction<FunctionType, OtherAllocator>&& other) noexcept
        : MoveOnlyScopedFunction{
              allocator,
              other.scope_state_,
              details::AllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>, Allocator>{
                  std::allocator_arg,
                  allocator,
                  std::move(other.callable_)}}
    {
    }

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
struct uses_allocator<score::safecpp::MoveOnlyScopedFunction<FunctionType, Alloc>, Alloc> : std::true_type
{
};

}  // namespace std

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_MOVE_ONLY_SCOPED_FUNCTION_H
