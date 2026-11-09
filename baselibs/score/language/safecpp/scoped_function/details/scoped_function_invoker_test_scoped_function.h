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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_SCOPED_FUNCTION_INVOKER_TEST_SCOPED_FUNCTION_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_SCOPED_FUNCTION_INVOKER_TEST_SCOPED_FUNCTION_H

#include "score/language/safecpp/scoped_function/details/allocator_aware_type_erasure_pointer.h"
#include "score/language/safecpp/scoped_function/details/function_wrapper.h"
#include "score/language/safecpp/scoped_function/details/invoker.h"
#include "score/language/safecpp/scoped_function/details/modify_signature.h"
#include "score/language/safecpp/scoped_function/details/scope_state.h"
#include "score/language/safecpp/scoped_function/details/scoped_function_invoker.h"

#include <score/stop_token.hpp>
#include <score/type_traits.hpp>

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

namespace score::safecpp
{

template <class FunctionType, class Allocator = std::allocator<std::uint8_t>>
class ScopedFunctionInvokerTestScopedFunction
    : public details::Invoker<details::ScopedFunctionInvoker,
                              details::CallOperatorInterface,
                              ScopedFunctionInvokerTestScopedFunction<FunctionType, Allocator>,
                              details::ModifySignatureT<FunctionType>>
{
  private:
    using BaseClass = details::Invoker<details::ScopedFunctionInvoker,
                                       details::CallOperatorInterface,
                                       ScopedFunctionInvokerTestScopedFunction<FunctionType, Allocator>,
                                       details::ModifySignatureT<FunctionType>>;

    template <class Callable>
    using FunctionWrapperImpl =
        typename details::FunctionWrapper<FunctionType>::template Implementation<score::cpp::remove_cvref_t<Callable>>;

    ScopedFunctionInvokerTestScopedFunction(
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

    ScopedFunctionInvokerTestScopedFunction() noexcept : ScopedFunctionInvokerTestScopedFunction{Allocator{}} {}
    ~ScopedFunctionInvokerTestScopedFunction() noexcept override = default;

    template <class Callable>
    explicit ScopedFunctionInvokerTestScopedFunction(std::shared_ptr<details::ScopeState> scope_state,
                                                     Callable&& callable) noexcept
        : allocator_{Allocator{}},
          scope_state_{std::move(scope_state)},
          callable_{
              details::MakeAllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>,
                                                            FunctionWrapperImpl<Callable>,
                                                            Allocator>(Allocator{}, std::forward<Callable>(callable))}
    {
    }

    explicit ScopedFunctionInvokerTestScopedFunction(
        std::shared_ptr<details::ScopeState> scope_state,
        details::AllocatorAwareTypeErasurePointer<details::FunctionWrapper<FunctionType>, Allocator> callable) noexcept
        : allocator_{Allocator{}}, scope_state_{std::move(scope_state)}, callable_{std::move(callable)}
    {
    }

    ScopedFunctionInvokerTestScopedFunction(const ScopedFunctionInvokerTestScopedFunction& other) = default;
    ScopedFunctionInvokerTestScopedFunction& operator=(const ScopedFunctionInvokerTestScopedFunction& other) = default;
    ScopedFunctionInvokerTestScopedFunction(ScopedFunctionInvokerTestScopedFunction&& other) noexcept = default;
    ScopedFunctionInvokerTestScopedFunction& operator=(ScopedFunctionInvokerTestScopedFunction&& other) noexcept =
        default;

  private:
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Friendship is the best way to gracefully support all variants of the function call operator correctly
    template <bool, class>
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class details::ScopedFunctionInvoker;

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
struct uses_allocator<score::safecpp::ScopedFunctionInvokerTestScopedFunction<FunctionType, Alloc>, Alloc>
    : std::true_type
{
};

}  // namespace std

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_SCOPED_FUNCTION_INVOKER_TEST_SCOPED_FUNCTION_H
