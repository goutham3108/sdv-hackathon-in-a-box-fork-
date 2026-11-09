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
#include "score/result/details/expected/expected.h"

#include "score/result/details/expected/test_type_traits.h"

#include <include/gtest/gtest.h>

#include <type_traits>
#include <utility>

namespace score::details
{
namespace
{

TEST(UnexpectTest, UnexpectT_InitializesExplicit)
{
    static_assert(is_only_explicitly_constructible_v<unexpect_t>, "unexpect_t must be only explicit constructible");
}

TEST(UnexpectTest, ProvidesHelperUnexpect)
{
    static_assert(std::is_same_v<decltype(unexpect), const unexpect_t>, "unexpect must be of type const unexpect_t");
}

class CopyableType
{
  public:
    explicit CopyableType(std::int32_t value) : value_{value} {}

    friend constexpr bool operator==(const CopyableType& lhs, const CopyableType& rhs)
    {
        return lhs.value_ == rhs.value_;
    }

    friend constexpr bool operator!=(const CopyableType& lhs, const CopyableType& rhs)
    {
        return lhs.value_ != rhs.value_;
    }

    int value_;
};

class MoveOnlyType
{
  public:
    explicit MoveOnlyType(std::int32_t value) : value_{value} {}
    MoveOnlyType(const MoveOnlyType&) = delete;
    MoveOnlyType(MoveOnlyType&&) = default;
    MoveOnlyType& operator=(const MoveOnlyType&) = delete;
    MoveOnlyType& operator=(MoveOnlyType&&) = default;

    std::int32_t value_;
};

TEST(UnexpectedTest, IsCopyConstructible)
{
    // Given an unexpected that is copyable
    std::int32_t value{15};
    unexpected unit{CopyableType{value}};

    // When doing a copy
    auto copy{unit};

    // Expect that their errors equal the original value
    EXPECT_EQ(unit.error().value_, value);
    EXPECT_EQ(copy.error().value_, value);
}

TEST(UnexpectedTest, IsMoveConstructible)
{
    // Given an unexpected that is move-only
    std::int32_t value{13};
    unexpected unit{MoveOnlyType{value}};

    // When moving
    auto moved{std::move(unit)};

    // Expect the moved error to equal the original value
    EXPECT_EQ(moved.error().value_, value);
}

TEST(UnexpectedTest, CanBeConstructedFromCompatibleType)
{
    // Given some value
    std::int32_t value{16};

    // When constructing the unexpected with a compatible type
    unexpected<CopyableType> unit{value};

    // Expect the unexepected to hold the value as error
    EXPECT_EQ(unit.error().value_, value);
}

TEST(UnexpectedTest, CanBeInPlaceConstructedFromCompatibleArguments)
{
    // Given a target type that is constructible with arguments based on two other types
    struct TargetType
    {
        explicit TargetType(const CopyableType& copyable_type, MoveOnlyType&& moveonly_type)
            : copyable_type_{copyable_type}, moveonly_type_{std::move(moveonly_type)}
        {
        }

        CopyableType copyable_type_;
        MoveOnlyType moveonly_type_;
    };

    std::int32_t copyable_value{14};
    std::int32_t moveonly_value{19};

    // And an unexpected constructed in place from the arguments
    unexpected<TargetType> unit{std::in_place, CopyableType{copyable_value}, MoveOnlyType{moveonly_value}};

    // Expect that the unexpected holds the arguments
    EXPECT_EQ(unit.error().copyable_type_.value_, copyable_value);
    EXPECT_EQ(unit.error().moveonly_type_.value_, moveonly_value);
}

TEST(UnexpectedTest, CanBeInPlaceConstructedFromInitializerList)
{
    // Given a target type that is constructible with an initializer list and other arguments based on two other types
    struct TargetType
    {
        explicit TargetType(std::initializer_list<CopyableType> copyable_type, MoveOnlyType&& moveonly_type)
            : copyable_type_{*copyable_type.begin()}, moveonly_type_{std::move(moveonly_type)}
        {
        }

        CopyableType copyable_type_;
        MoveOnlyType moveonly_type_;
    };

    std::int32_t copyable_value{14};
    std::int32_t moveonly_value{19};

    // And an unexpected constructed in place from an initializer list and the additional argument
    unexpected<TargetType> unit{std::in_place, {CopyableType{copyable_value}}, MoveOnlyType{moveonly_value}};

    // Expect that the unexpected holds the arguments
    EXPECT_EQ(unit.error().copyable_type_.value_, copyable_value);
    EXPECT_EQ(unit.error().moveonly_type_.value_, moveonly_value);
}

TEST(UnexpectedTest, IsCopyAssignable)
{
    // Given two unexpected with different values
    std::int32_t value{15};
    unexpected unit{CopyableType{value}};
    std::int32_t old_value{16};
    unexpected<CopyableType> copy{old_value};
    EXPECT_EQ(copy.error().value_, old_value);

    // When the first is copy assigned to the second
    copy = unit;

    // Expect both to hold the same values
    EXPECT_EQ(unit.error().value_, value);
    EXPECT_EQ(copy.error().value_, value);
}

TEST(UnexpectedTest, IsMoveAssignable)
{
    // Given two unexpected with different values
    std::int32_t value{13};
    unexpected unit{MoveOnlyType{value}};
    std::int32_t old_value{16};
    unexpected<MoveOnlyType> moved{old_value};
    EXPECT_EQ(moved.error().value_, old_value);

    // When the first is move assigned to the second
    moved = std::move(unit);

    // Expect the value of the first transferred to the last
    EXPECT_EQ(moved.error().value_, value);
}

TEST(UnexpectedTest, CanRetrieveErrorFromConstLValueReference)
{
    // Given an unexpected with a value
    std::int32_t value{13};
    const unexpected unit{CopyableType{value}};

    // When using the unexpected as const-lvalue-reference the error matches the original value
    EXPECT_EQ(unit.error().value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(unit.error()), const CopyableType&>);
}

TEST(UnexpectedTest, CanRetrieveErrorFromLValueReference)
{
    // Given an unexpected with a value
    std::int32_t value{13};
    unexpected unit{CopyableType{value}};

    // When using the unexpected as lvalue-reference the error matches the original value
    auto& ref = unit;

    // When using the unexpected as lvalue-reference the error matches the original value
    EXPECT_EQ(ref.error().value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(ref.error()), CopyableType&>);
}

TEST(UnexpectedTest, CanRetrieveErrorFromConstRValueReference)
{
    // Given an unexpected with a value
    std::int32_t value{13};
    const unexpected unit{MoveOnlyType{value}};

    // When using the unexpected as const-rvalue-reference the error matches the original value
    EXPECT_EQ(std::move(unit).error().value_, value);

    // Expect to be able to retrieve the error with correct type
    // Double move fine because the operand of decltype is unevaluated:
    // https://timsong-cpp.github.io/cppwp/n4659/dcl.spec#dcl.type.simple-4
    static_assert(std::is_same_v<decltype(std::move(unit).error()), const MoveOnlyType&&>);
}

TEST(UnexpectedTest, CanRetrieveErrorFromRValueReference)
{
    // Given an unexpected with a value
    std::int32_t value{13};
    unexpected unit{MoveOnlyType{value}};

    // When using the unexpected as rvalue-reference the error matches the original value
    EXPECT_EQ(std::move(unit).error().value_, value);

    // Expect to be able to retrieve the error with correct type
    // Double move fine because the operand of decltype is unevaluated:
    // https://timsong-cpp.github.io/cppwp/n4659/dcl.spec#dcl.type.simple-4
    static_assert(std::is_same_v<decltype(std::move(unit).error()), MoveOnlyType&&>);
}

TEST(UnexpectedTest, CanCompareEquality)
{
    // Given an unexpected
    std::int32_t value{13};
    unexpected lhs{CopyableType{value}};

    // And a second unexpected with equal value
    unexpected rhs_same{CopyableType{value}};

    // And a third unexpected with a different value
    unexpected rhs_different{CopyableType{value + 1}};

    // Expect the equality operators to compare correctly
    EXPECT_TRUE(lhs == rhs_same);
    EXPECT_FALSE(lhs != rhs_same);
    EXPECT_FALSE(lhs == rhs_different);
    EXPECT_TRUE(lhs != rhs_different);
}

TEST(UnexpectedTest, CanSwapWithStdSwap)
{
    // Given two unexpected with different values
    std::int32_t value{13};
    unexpected unit{CopyableType{value}};
    std::int32_t other_value{15};
    unexpected other_unit{CopyableType{other_value}};

    // When swapping them with std::swap
    std::swap(unit, other_unit);

    // Expect them to be swapped
    EXPECT_EQ(unit.error().value_, other_value);
    EXPECT_EQ(other_unit.error().value_, value);
}

TEST(UnexpectedTest, CanSwapWithTypeSwap)
{
    // Given two unexpected with different values
    std::int32_t value{13};
    unexpected unit{CopyableType{value}};
    std::int32_t other_value{15};
    unexpected other_unit{CopyableType{other_value}};

    // When swapping them with member function swap
    unit.swap(other_unit);

    // Expect them to be swapped
    EXPECT_EQ(unit.error().value_, other_value);
    EXPECT_EQ(other_unit.error().value_, value);
}

TEST(UnexpectedTest, DeductionGuideIsSpecified)
{
    // Given an unexpected that was constructed using the type deduction guide
    std::int32_t value{13};
    unexpected unit{CopyableType{value}};

    // Expect the resulting type to be correct
    static_assert(std::is_same_v<decltype(unit), unexpected<CopyableType>>);
}

}  // namespace
}  // namespace score::details
