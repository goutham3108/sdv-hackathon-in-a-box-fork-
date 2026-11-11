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
#include "score/language/safecpp/scoped_function/copyable_scoped_function.h"
#include "score/language/safecpp/scoped_function/move_only_scoped_function.h"

#include "score/language/safecpp/scoped_function/details/instrumented_memory_resource.h"
#include "score/language/safecpp/scoped_function/details/parametrization_helper.h"
#include "score/language/safecpp/scoped_function/details/testing_allocator.h"
#include "score/language/safecpp/scoped_function/scope.h"

#include <score/memory.hpp>
#include <score/stop_token.hpp>

#include <gtest/gtest.h>

#include <type_traits>

namespace score::safecpp
{
namespace
{

template <class T>
class ScopedFunctionBasicTest : public ::testing::Test
{
  public:
    score::cpp::stop_source stop_source_;
};

using Types =
    ::testing::Types<ParametrizationHelper<CopyableScopedFunction>, ParametrizationHelper<MoveOnlyScopedFunction>>;
TYPED_TEST_SUITE(ScopedFunctionBasicTest, Types, /* unused */);

TYPED_TEST(ScopedFunctionBasicTest, CanConstructUsingNonPolymorphicAllocator)
{
    using allocator_type = std::allocator<std::uint8_t>;
    using ScopedFunction = typename TypeParam::template Type<void(), allocator_type>;
    static_assert(std::uses_allocator_v<ScopedFunction, allocator_type>);
}

TYPED_TEST(ScopedFunctionBasicTest, CanConstructConstSpezializationUsingNonPolymorphicAllocator)
{
    using allocator_type = std::allocator<std::uint8_t>;
    using ScopedFunction = typename TypeParam::template Type<void() const, allocator_type>;
    static_assert(std::uses_allocator_v<ScopedFunction, allocator_type>, "Must be allocator-aware");
}

TYPED_TEST(ScopedFunctionBasicTest, CanConstructUsingPolymorphicAllocator)
{
    using allocator_type = score::cpp::pmr::polymorphic_allocator<>;
    using ScopedFunction = typename TypeParam::template Type<void(), allocator_type>;
    static_assert(std::uses_allocator_v<ScopedFunction, allocator_type>, "Must be allocator-aware");

    InstrumentedMemoryResource resource{};
    score::cpp::pmr::polymorphic_allocator<> allocator{&resource};

    Scope<> scope{};
    bool called{false};

    auto function = score::cpp::make_obj_using_allocator<ScopedFunction>(allocator, scope, [&called]() {
        called = true;
    });

    ASSERT_EQ(resource.GetNumberOfAllocations(), 1);

    function();
    EXPECT_TRUE(called);
}

TYPED_TEST(ScopedFunctionBasicTest, CanConstructAndDestructWithoutScope)
{
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{};
}

TYPED_TEST(ScopedFunctionBasicTest, FunctionWithoutScopeIsImmediatelyExpired)
{
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{};
    EXPECT_FALSE(function());
}

TYPED_TEST(ScopedFunctionBasicTest, CanConstructAndDestructWithScopeAndRValueLambda)
{
    Scope<> scope{};
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{scope, []() noexcept {}};
}

TYPED_TEST(ScopedFunctionBasicTest, CanConstructAndDestructWithStopTokenAndRValueLambda)
{
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{this->stop_source_.get_token(), []() noexcept {}};
}

TYPED_TEST(ScopedFunctionBasicTest, CanConstructAndDestructWithScopeAndLValueLambda)
{
    Scope<> scope{};
    const auto lambda = []() noexcept {};
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{scope, lambda};
}

TYPED_TEST(ScopedFunctionBasicTest, CanConstructAndDestructWithStopTokenAndLValueLambda)
{
    const auto lambda = []() noexcept {};
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{this->stop_source_.get_token(), lambda};
}

TYPED_TEST(ScopedFunctionBasicTest, AttachesItselfToScope)
{
    bool called{false};
    Scope<> scope{};
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{scope, [&called]() noexcept {
                                called = true;
                            }};
    scope.Expire();
    function();
    EXPECT_FALSE(called);
}

TYPED_TEST(ScopedFunctionBasicTest, AttachesItselfToStopSource)
{
    bool called{false};
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{this->stop_source_.get_token(), [&called]() noexcept {
                                called = true;
                            }};
    this->stop_source_.request_stop();
    function();
    EXPECT_FALSE(called);
}

TYPED_TEST(ScopedFunctionBasicTest, AttachesItselfToStopSourceAndIsImmediatelyExpiredIfStopWasAlreadyRequested)
{
    this->stop_source_.request_stop();
    bool called{false};
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{this->stop_source_.get_token(), [&called]() noexcept {
                                called = true;
                            }};
    function();
    EXPECT_FALSE(called);
}

TYPED_TEST(ScopedFunctionBasicTest, CanDestructWithAlreadyDestroyedScope)
{
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction outer_function{};
    {
        Scope<> scope{};
        outer_function = ScopedFunction{scope, []() noexcept {}};
    }
    EXPECT_FALSE(outer_function());
}

TYPED_TEST(ScopedFunctionBasicTest, EqualsTrueWithEnclosedCallable)
{
    Scope<> scope{};
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{scope, []() noexcept {}};

    EXPECT_TRUE(function.operator bool());
}

TYPED_TEST(ScopedFunctionBasicTest, EqualsFalseWithoutEnclosedCallable)
{
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{};

    EXPECT_FALSE(function.operator bool());
}

TYPED_TEST(ScopedFunctionBasicTest, CanGetAllocator)
{
    Scope<> scope{};
    using Allocator = details::TestingAllocator<std::byte, std::false_type>;
    Allocator allocator{};
    using ScopedFunction = typename TypeParam::template Type<void(), Allocator>;
    ScopedFunction function{std::allocator_arg, allocator, scope, []() noexcept {}};

    EXPECT_EQ(function.get_allocator(), allocator);
}

class ScopedFunctionBasicUnparametrizedTest : public ::testing::Test
{
};

TEST_F(ScopedFunctionBasicUnparametrizedTest, CanConstructMoveOnlyFromCopyableScopedFunction)
{
    bool called{false};
    Scope<> scope{};
    CopyableScopedFunction<void()> copyable_function{scope, [&called]() noexcept {
                                                         called = true;
                                                     }};
    MoveOnlyScopedFunction<void()> move_only_function{std::move(copyable_function)};
    move_only_function();
    EXPECT_TRUE(called);
}

TEST_F(ScopedFunctionBasicUnparametrizedTest, CanConstructMoveOnlyFromCopyableScopedFunctionWithAllocator)
{
    bool called{false};
    Scope<> scope{};
    CopyableScopedFunction<void()> copyable_function{scope, [&called]() noexcept {
                                                         called = true;
                                                     }};

    InstrumentedMemoryResource resource{};
    score::cpp::pmr::polymorphic_allocator<> allocator{&resource};
    MoveOnlyScopedFunction<void(), decltype(allocator)> move_only_function{std::move(copyable_function), allocator};

    ASSERT_EQ(resource.GetNumberOfAllocations(), 1);

    move_only_function();
    EXPECT_TRUE(called);
}

}  // namespace
}  // namespace score::safecpp
