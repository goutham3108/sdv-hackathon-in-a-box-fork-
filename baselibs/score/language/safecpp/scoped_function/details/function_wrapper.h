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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_FUNCTION_WRAPPER_FUNCTION_WRAPPER_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_FUNCTION_WRAPPER_FUNCTION_WRAPPER_H

#include "score/language/safecpp/scoped_function/details/allocator_aware_erased_type.h"
#include "score/language/safecpp/scoped_function/details/invoker.h"

#include "score/memory.hpp"

#include <type_traits>
#include <utility>

namespace score::safecpp::details
{

template <bool NoExcept, class FunctionType>
class FunctionWrapperInvoker;

template <bool NoExcept, class ReturnType, class... Args>
class FunctionWrapperInvoker<NoExcept, ReturnType(Args...)>
{
  public:
    template <class FunctionWrapper>
    using CallableType = ReturnType (*)(FunctionWrapper&, Args...) noexcept(NoExcept);

    template <class FunctionWrapper,
              class LocalReturnType = ReturnType,
              std::enable_if_t<!std::is_void_v<LocalReturnType>, bool> = true>
    [[nodiscard]] static CallableType<FunctionWrapper> ActualInvoker() noexcept
    {
        return [](FunctionWrapper& container, Args... args) -> ReturnType {
            return container.callable_(std::forward<Args>(args)...);
        };
    }

    template <class FunctionWrapper,
              class LocalReturnType = ReturnType,
              std::enable_if_t<std::is_void_v<LocalReturnType>, bool> = true>
    [[nodiscard]] static CallableType<FunctionWrapper> ActualInvoker() noexcept
    {
        return [](FunctionWrapper& container, Args... args) -> ReturnType {
            container.callable_(std::forward<Args>(args)...);
        };
    }
};

template <class Callable, class FunctionType>
class FunctionWrapperImpl;

// We intentionally use multi-inheritance to make this class compatible with an allocation-aware erased
// type and provide call operator interfaces. Since both base classes are pure abstract and do not share a common base
// class, we avoid the dreaded diamond inheritance.
//
// From an interview with Bjarne Stroustrup (https://www.artima.com/articles/modern-c-style):
// "I've seen cases where multiple inheritance is useful, and I've even seen cases where quite complicated multiple
// inheritance is useful. Generally, I prefer to use the facilities offered by the language to doing workarounds."

template <class FunctionType>
class FunctionWrapper : public CallOperatorInterface<FunctionType>,
                        public AllocatorAwareErasedType<FunctionWrapper<FunctionType>>
{
  public:
    using ReturnType = typename details::CallOperatorInterface<FunctionType>::ReturnType;

    // Suppress "AUTOSAR C++14 A5-1-7" rule finding.
    // This rule states: "A lambda shall not be an operand to decltype or typeid.".
    template <class Callable>
    // coverity[autosar_cpp14_a5_1_7_violation : FALSE]: Neither decltype nor typeid is used here.
    using Implementation = FunctionWrapperImpl<Callable, FunctionType>;

  protected:
    constexpr FunctionWrapper() noexcept = default;
    constexpr FunctionWrapper(const FunctionWrapper&) = default;
    constexpr FunctionWrapper(FunctionWrapper&&) noexcept = default;
    constexpr FunctionWrapper& operator=(const FunctionWrapper&) = default;
    constexpr FunctionWrapper& operator=(FunctionWrapper&&) noexcept = default;

  public:
    ~FunctionWrapper() override = default;
};

template <class Callable, class FunctionType>
class FunctionWrapperImpl : public details::Invoker<details::FunctionWrapperInvoker,
                                                    FunctionWrapper,
                                                    FunctionWrapperImpl<Callable, FunctionType>,
                                                    FunctionType>
{
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding.
    // This rule states: "A lambda shall not be an operand to decltype or typeid.".
    // coverity[autosar_cpp14_a5_1_7_violation : FALSE]: Neither decltype nor typeid is used here.
    using BaseClass = details::Invoker<details::FunctionWrapperInvoker,
                                       FunctionWrapper,
                                       FunctionWrapperImpl<Callable, FunctionType>,
                                       FunctionType>;

  public:
    template <
        class LocalCallable = Callable,
        std::enable_if_t<!std::is_move_constructible_v<LocalCallable> && std::is_copy_constructible_v<LocalCallable>,
                         bool> = true>
    explicit FunctionWrapperImpl(Callable callable) noexcept : BaseClass{}, callable_{callable}
    {
    }

    template <class LocalCallable = Callable,
              std::enable_if_t<std::is_move_constructible_v<LocalCallable>, bool> = true>
    explicit FunctionWrapperImpl(Callable callable) noexcept : BaseClass{}, callable_{std::forward<Callable>(callable)}
    {
    }

    [[nodiscard]] TypeErasurePointer<FunctionWrapper<FunctionType>> Copy(
        score::cpp::pmr::polymorphic_allocator<FunctionWrapper<FunctionType>> allocator) const override
    {
        return DoCopy(allocator);
    }

    [[nodiscard]] TypeErasurePointer<FunctionWrapper<FunctionType>> Move(
        score::cpp::pmr::polymorphic_allocator<FunctionWrapper<FunctionType>> allocator) noexcept override
    {
        return DoMove(allocator);
    }

    using details::Invoker<details::FunctionWrapperInvoker,
                           FunctionWrapper,
                           FunctionWrapperImpl<Callable, FunctionType>,
                           FunctionType>::operator();

  private:
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Friendship is the best way to gracefully support all variants of the function call operator correctly
    template <bool, class>
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class details::FunctionWrapperInvoker;

    // Suppress "AUTOSAR C++14 A5-1-7" rule finding.
    // This rule states: "A lambda shall not be an operand to decltype or typeid.".
    // coverity[autosar_cpp14_a5_1_7_violation : FALSE]: Neither decltype nor typeid is used here.
    Callable callable_;

    template <class LocalCallable = Callable,
              std::enable_if_t<std::is_copy_constructible_v<LocalCallable>, bool> = true>
    [[nodiscard]] TypeErasurePointer<FunctionWrapper<FunctionType>> DoCopy(
        score::cpp::pmr::polymorphic_allocator<FunctionWrapper<FunctionType>> allocator) const
    {
        return MakeTypeErasurePointer<FunctionWrapperImpl>(allocator, this->callable_);
    }

    template <class LocalCallable = Callable,
              std::enable_if_t<!std::is_copy_constructible_v<LocalCallable>, bool> = true>
    [[nodiscard]] TypeErasurePointer<FunctionWrapper<FunctionType>> DoCopy(
        score::cpp::pmr::polymorphic_allocator<FunctionWrapper<FunctionType>>) const
    {
        // This code is unreachable by public API because:
        // - CopyableScopedFunction forces callables to be CopyConstructible
        // - MoveOnlyScopedFunction calls only DoMove() directly and can not call the variant that forwards to DoCopy(),
        //   since that variant forces the callable to be CopyConstructible
        std::terminate();
    }

    template <class LocalCallable = Callable,
              std::enable_if_t<std::is_move_constructible_v<LocalCallable>, bool> = true>
    [[nodiscard]] TypeErasurePointer<FunctionWrapper<FunctionType>> DoMove(
        score::cpp::pmr::polymorphic_allocator<FunctionWrapper<FunctionType>> allocator) noexcept
    {
        return MakeTypeErasurePointer<FunctionWrapperImpl>(allocator, std::move(this->callable_));
    }

    template <
        class LocalCallable = Callable,
        std::enable_if_t<!std::is_move_constructible_v<LocalCallable> && std::is_copy_constructible_v<LocalCallable>,
                         bool> = true>
    [[nodiscard]] TypeErasurePointer<FunctionWrapper<FunctionType>> DoMove(
        score::cpp::pmr::polymorphic_allocator<FunctionWrapper<FunctionType>> allocator) const noexcept
    {
        return Copy(allocator);
    }
};

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_FUNCTION_WRAPPER_FUNCTION_WRAPPER_H
