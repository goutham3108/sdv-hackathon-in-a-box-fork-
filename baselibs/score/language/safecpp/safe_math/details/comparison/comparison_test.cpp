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
#include "score/language/safecpp/safe_math/details/comparison/comparison.h"
#include "score/language/safecpp/safe_math/details/test_type_collection.h"

#include <gtest/gtest.h>

#include <score/math.hpp>

#include <cstdint>
#include <limits>
#include <utility>

namespace score::safe_math
{
namespace
{

template <typename>
class CmpTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(CmpTest);

TYPED_TEST_P(CmpTest, CmpEqualReturnsTrueWithBothZero)
{
    const typename TypeParam::first_type first_type_zero{0};
    const typename TypeParam::second_type second_type_zero{0};
    EXPECT_TRUE(CmpEqual(first_type_zero, second_type_zero)) << first_type_zero << " == " << second_type_zero;
}

TYPED_TEST_P(CmpTest, CmpEqualReturnsFalseWithLowestAgainstOne)
{
    const auto first_type_lowest{std::numeric_limits<typename TypeParam::first_type>::lowest()};
    const typename TypeParam::second_type second_type_one{1};
    EXPECT_FALSE(CmpEqual(first_type_lowest, second_type_one)) << first_type_lowest << " == " << second_type_one;
}

TYPED_TEST_P(CmpTest, CmpEqualReturnsFalseWithOneAgainstLowest)
{
    const typename TypeParam::first_type first_type_one{1};
    const auto second_type_lowest{std::numeric_limits<typename TypeParam::second_type>::lowest()};
    EXPECT_FALSE(CmpEqual(first_type_one, second_type_lowest)) << first_type_one << " == " << second_type_lowest;
}

TYPED_TEST_P(CmpTest, CmpEqualReturnsFalseWithTwoAgainstThree)
{
    const auto first_type_two{static_cast<typename TypeParam::first_type>(2)};
    const auto second_type_three{static_cast<typename TypeParam::second_type>(3)};
    EXPECT_FALSE(CmpEqual(first_type_two, second_type_three)) << first_type_two << " == " << second_type_three;
}

TYPED_TEST_P(CmpTest, CmpNotEqualReturnsTrueWithTwoAgainstThree)
{
    const auto first_type_two{static_cast<typename TypeParam::first_type>(2)};
    const auto second_type_three{static_cast<typename TypeParam::second_type>(3)};
    EXPECT_TRUE(CmpNotEqual(first_type_two, second_type_three)) << first_type_two << " != " << second_type_three;
}

TYPED_TEST_P(CmpTest, CmpNotEqualReturnsFalseWithTwoAgainstTwo)
{
    const auto first_type_two{static_cast<typename TypeParam::first_type>(2)};
    const auto second_type_two{static_cast<typename TypeParam::second_type>(first_type_two)};
    EXPECT_FALSE(CmpNotEqual(first_type_two, second_type_two)) << first_type_two << " != " << second_type_two;
}

TYPED_TEST_P(CmpTest, CmpNotEqualReturnsTrueWithLowestAgainstHighest)
{
    const auto first_type_two{std::numeric_limits<typename TypeParam::first_type>::lowest()};
    const auto second_type_two{std::numeric_limits<typename TypeParam::second_type>::max()};
    EXPECT_TRUE(CmpNotEqual(first_type_two, second_type_two)) << first_type_two << " != " << second_type_two;
}

TYPED_TEST_P(CmpTest, CmpLessReturnsTrueWithZeroAndOne)
{
    const typename TypeParam::first_type first_type_zero{0};
    const typename TypeParam::second_type second_type_one{1};
    EXPECT_TRUE(CmpLess(first_type_zero, second_type_one)) << first_type_zero << " < " << second_type_one;
}

TYPED_TEST_P(CmpTest, CmpLessReturnsFalseWithZeroAndZero)
{
    const typename TypeParam::first_type first_type_zero{0};
    const typename TypeParam::second_type second_type_zero{0};
    EXPECT_FALSE(CmpLess(first_type_zero, second_type_zero)) << first_type_zero << " < " << second_type_zero;
}

TYPED_TEST_P(CmpTest, CmpLessReturnsFalseWithOneAndZero)
{
    const typename TypeParam::first_type first_type_one{1};
    const typename TypeParam::second_type second_type_zero{0};
    EXPECT_FALSE(CmpLess(first_type_one, second_type_zero)) << first_type_one << " < " << second_type_zero;
}

TYPED_TEST_P(CmpTest, CmpLessReturnsTrueWithMinusOneAndZero)
{
    if (!std::is_signed<typename TypeParam::first_type>::value)
    {
        GTEST_SKIP();
    }

    const auto first_type_minus_one{static_cast<typename TypeParam::first_type>(-1)};
    const typename TypeParam::second_type second_type_zero{0};
    EXPECT_TRUE(CmpLess(first_type_minus_one, second_type_zero)) << first_type_minus_one << " < " << second_type_zero;
}

TYPED_TEST_P(CmpTest, CmpLessReturnsFalseWithZeroAndMinusOne)
{
    if (!std::is_signed<typename TypeParam::second_type>::value)
    {
        GTEST_SKIP();
    }

    const typename TypeParam::first_type first_type_zero{0};
    const auto second_type_minus_one{static_cast<typename TypeParam::second_type>(-1)};
    EXPECT_FALSE(CmpLess(first_type_zero, second_type_minus_one)) << first_type_zero << " < " << second_type_minus_one;
}

TYPED_TEST_P(CmpTest, CmpLessReturnsTrueWithLowestAgainstMax)
{
    const auto first_type_lowest{std::numeric_limits<typename TypeParam::first_type>::lowest()};
    const auto second_type_max{std::numeric_limits<typename TypeParam::second_type>::max()};
    EXPECT_TRUE(CmpLess(first_type_lowest, second_type_max)) << first_type_lowest << " < " << second_type_max;
}

TYPED_TEST_P(CmpTest, CmpLessReturnsFalseWithMaxAgainstLowest)
{
    const auto first_type_max{std::numeric_limits<typename TypeParam::first_type>::max()};
    const auto second_type_lowest{std::numeric_limits<typename TypeParam::second_type>::lowest()};
    EXPECT_FALSE(CmpLess(first_type_max, second_type_lowest)) << first_type_max << " < " << second_type_lowest;
}

TYPED_TEST_P(CmpTest, CmpGreaterReturnsFalseWithZeroAgainstZero)
{
    const typename TypeParam::first_type first_type_zero{0};
    const typename TypeParam::second_type second_type_zero{0};
    EXPECT_FALSE(CmpGreater(first_type_zero, second_type_zero)) << first_type_zero << " > " << second_type_zero;
}

TYPED_TEST_P(CmpTest, CmpGreaterReturnsFalseWithLowestAgainstMax)
{
    const auto first_type_lowest{std::numeric_limits<typename TypeParam::first_type>::lowest()};
    const auto second_type_max{std::numeric_limits<typename TypeParam::second_type>::max()};
    EXPECT_FALSE(CmpGreater(first_type_lowest, second_type_max)) << first_type_lowest << " > " << second_type_max;
}

TYPED_TEST_P(CmpTest, CmpGreaterReturnsTrueWithMaxAgainstLowest)
{
    const auto first_type_max{std::numeric_limits<typename TypeParam::first_type>::max()};
    const auto second_type_lowest{std::numeric_limits<typename TypeParam::second_type>::lowest()};
    EXPECT_TRUE(CmpGreater(first_type_max, second_type_lowest)) << first_type_max << " > " << second_type_lowest;
}

TYPED_TEST_P(CmpTest, CmpLessEqualReturnsTrueWithZeroAgainstOne)
{
    const typename TypeParam::first_type first_type_zero{0};
    const typename TypeParam::second_type second_type_one{1};
    EXPECT_TRUE(CmpLessEqual(first_type_zero, second_type_one)) << first_type_zero << " <= " << second_type_one;
}

TYPED_TEST_P(CmpTest, CmpLessEqualReturnsTrueWithZeroAgainstZero)
{
    const typename TypeParam::first_type first_type_zero{0};
    const typename TypeParam::second_type second_type_zero{0};
    EXPECT_TRUE(CmpLessEqual(first_type_zero, second_type_zero)) << first_type_zero << " <= " << second_type_zero;
}

TYPED_TEST_P(CmpTest, CmpLessEqualReturnsFalseWithOneAgainstZero)
{
    const typename TypeParam::first_type first_type_one{1};
    const typename TypeParam::second_type second_type_zero{0};
    EXPECT_FALSE(CmpLessEqual(first_type_one, second_type_zero)) << first_type_one << " <= " << second_type_zero;
}

TYPED_TEST_P(CmpTest, CmpLessEqualReturnsTrueWithLowestAgainstHighest)
{
    const auto first_type_lowest{std::numeric_limits<typename TypeParam::first_type>::lowest()};
    const auto second_type_max{std::numeric_limits<typename TypeParam::second_type>::max()};
    EXPECT_TRUE(CmpLessEqual(first_type_lowest, second_type_max)) << first_type_lowest << " <= " << second_type_max;
}

TYPED_TEST_P(CmpTest, CmpLessEqualReturnsFalseWithHighestAgainstLowest)
{
    const auto first_type_max{std::numeric_limits<typename TypeParam::first_type>::max()};
    const auto second_type_lowest{std::numeric_limits<typename TypeParam::second_type>::lowest()};
    EXPECT_FALSE(CmpLessEqual(first_type_max, second_type_lowest)) << first_type_max << " <= " << second_type_lowest;
}

TYPED_TEST_P(CmpTest, CmpGreaterEqualReturnsFalseWithZeroAgainstOne)
{
    const typename TypeParam::first_type first_type_zero{0};
    const typename TypeParam::second_type second_type_one{1};
    EXPECT_FALSE(CmpGreaterEqual(first_type_zero, second_type_one)) << first_type_zero << " >= " << second_type_one;
}

TYPED_TEST_P(CmpTest, CmpGreaterEqualReturnsTrueWithZeroAgainstZero)
{
    const typename TypeParam::first_type first_type_zero{0};
    const typename TypeParam::second_type second_type_zero{0};
    EXPECT_TRUE(CmpGreaterEqual(first_type_zero, second_type_zero)) << first_type_zero << " >= " << second_type_zero;
}

TYPED_TEST_P(CmpTest, CmpGreaterEqualReturnsTrueWithOneAgainstZero)
{
    const typename TypeParam::first_type first_type_one{1};
    const typename TypeParam::second_type second_type_zero{0};
    EXPECT_TRUE(CmpGreaterEqual(first_type_one, second_type_zero)) << first_type_one << " >= " << second_type_zero;
}

TYPED_TEST_P(CmpTest, CmpGreaterEqualReturnsFalseWithLowestAgainstHighest)
{
    const auto first_type_lowest{std::numeric_limits<typename TypeParam::first_type>::lowest()};
    const auto second_type_max{std::numeric_limits<typename TypeParam::second_type>::max()};
    EXPECT_FALSE(CmpGreaterEqual(first_type_lowest, second_type_max)) << first_type_lowest << " >= " << second_type_max;
}

TYPED_TEST_P(CmpTest, CmpGreaterEqualReturnsTrueWithHighestAgainstLowest)
{
    const auto first_type_max{std::numeric_limits<typename TypeParam::first_type>::max()};
    const auto second_type_lowest{std::numeric_limits<typename TypeParam::second_type>::lowest()};
    EXPECT_TRUE(CmpGreaterEqual(first_type_max, second_type_lowest)) << first_type_max << " >= " << second_type_lowest;
}

REGISTER_TYPED_TEST_SUITE_P(CmpTest,
                            CmpEqualReturnsTrueWithBothZero,
                            CmpEqualReturnsFalseWithLowestAgainstOne,
                            CmpEqualReturnsFalseWithOneAgainstLowest,
                            CmpEqualReturnsFalseWithTwoAgainstThree,
                            CmpNotEqualReturnsTrueWithTwoAgainstThree,
                            CmpNotEqualReturnsFalseWithTwoAgainstTwo,
                            CmpNotEqualReturnsTrueWithLowestAgainstHighest,
                            CmpLessReturnsTrueWithZeroAndOne,
                            CmpLessReturnsFalseWithZeroAndZero,
                            CmpLessReturnsFalseWithOneAndZero,
                            CmpLessReturnsTrueWithMinusOneAndZero,
                            CmpLessReturnsFalseWithZeroAndMinusOne,
                            CmpLessReturnsTrueWithLowestAgainstMax,
                            CmpLessReturnsFalseWithMaxAgainstLowest,
                            CmpGreaterReturnsFalseWithZeroAgainstZero,
                            CmpGreaterReturnsFalseWithLowestAgainstMax,
                            CmpGreaterReturnsTrueWithMaxAgainstLowest,
                            CmpLessEqualReturnsTrueWithZeroAgainstOne,
                            CmpLessEqualReturnsTrueWithZeroAgainstZero,
                            CmpLessEqualReturnsFalseWithOneAgainstZero,
                            CmpLessEqualReturnsTrueWithLowestAgainstHighest,
                            CmpLessEqualReturnsFalseWithHighestAgainstLowest,
                            CmpGreaterEqualReturnsFalseWithZeroAgainstOne,
                            CmpGreaterEqualReturnsTrueWithZeroAgainstZero,
                            CmpGreaterEqualReturnsTrueWithOneAgainstZero,
                            CmpGreaterEqualReturnsFalseWithLowestAgainstHighest,
                            CmpGreaterEqualReturnsTrueWithHighestAgainstLowest);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypes, CmpTest, UnsignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypes, CmpTest, SignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedUnsignedTypes, CmpTest, SignedUnsignedTypePairs, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedSignedTypes, CmpTest, UnsignedSignedTypePairs, /* unused */);

TEST(FloatingTypesSpecialTests, CmpEqualReturnsFalseIfLhsNan)
{
    const double lhs{std::numeric_limits<double>::quiet_NaN()};
    const double rhs{0.0};
    const std::int32_t tolerance{0};
    EXPECT_FALSE(CmpEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpEqualReturnsFalseIfRhsNan)
{
    const double lhs{0.0};
    const double rhs{std::numeric_limits<double>::quiet_NaN()};
    const std::int32_t tolerance{0};
    EXPECT_FALSE(CmpEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpEqualWithDefaultUlpsDoesRequiresPerfectEquality)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::max())};
    const std::int32_t tolerance{0};
    EXPECT_FALSE(CmpEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpEqualWithOneUlpToleranceReturnsTrueIfRhsIsOneUlpHigher)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::max())};
    const std::int32_t tolerance{1};
    EXPECT_TRUE(CmpEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpEqualWithOneUlpToleranceReturnsFalseIfRhsIsTwoUlpsHigher)
{
    const double lhs{1.0};
    double rhs{lhs};
    const std::int32_t tolerance{1};
    for (auto i{0}; i <= tolerance; ++i)
    {
        rhs = score::cpp::nextafter(rhs, std::numeric_limits<double>::max());
    }
    EXPECT_FALSE(CmpEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpEqualWithOneUlpToleranceReturnsTrueIfRhsIsOneUlpLower)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::lowest())};
    const std::int32_t tolerance{1};
    EXPECT_TRUE(CmpEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpEqualWithOneUlpToleranceReturnsFalseIfRhsIsTwoUlpsLower)
{
    const double lhs{1.0};
    double rhs{lhs};
    const std::int32_t tolerance{1};
    for (auto i{0}; i <= tolerance; ++i)
    {
        rhs = score::cpp::nextafter(rhs, std::numeric_limits<double>::lowest());
    }
    EXPECT_FALSE(CmpEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpNotEqualWillReturnFalseIfValueWithinUlpBounds)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::max())};
    const std::int32_t tolerance{1};
    EXPECT_FALSE(CmpNotEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpNotEqualWillReturnTrueIfValueOutsideUlpBounds)
{
    const double lhs{1.0};
    double rhs{lhs};
    const std::int32_t tolerance{1};
    for (auto i{0}; i <= tolerance; ++i)
    {
        rhs = score::cpp::nextafter(rhs, std::numeric_limits<double>::lowest());
    }
    EXPECT_TRUE(CmpNotEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpLessWillReturnTrueIfValueIsOneUlpLower)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::max())};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingTypesSpecialTests, CmpLessWillReturnFalseIfEqual)
{
    const double lhs{1.0};
    const double rhs{lhs};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingTypesSpecialTests, CmpGreaterWillReturnTrueIfValueIsOneUlpHigher)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::lowest())};
    EXPECT_TRUE(CmpGreater(lhs, rhs));
}

TEST(FloatingTypesSpecialTests, CmpGreaterWillReturnFalseIfEqual)
{
    const double lhs{1.0};
    const double rhs{lhs};
    EXPECT_FALSE(CmpGreater(lhs, rhs));
}

TEST(FloatingTypesSpecialTests, CmpGreaterEqualWillReturnTrueIfValueIsOneUlpHigher)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::lowest())};
    const std::int32_t tolerance{0};
    EXPECT_TRUE(CmpGreaterEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpGreaterEqualWillReturnTrueIfEqual)
{
    const double lhs{1.0};
    const double rhs{lhs};
    const std::int32_t tolerance{0};
    EXPECT_TRUE(CmpGreaterEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpGreaterEqualWillReturnTrueIfWithinTolerance)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::max())};
    std::int32_t tolerance{1};
    EXPECT_TRUE(CmpGreaterEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpGreaterEqualWillReturnFalseIfOutsideTolerance)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::max())};
    const std::int32_t tolerance{0};
    EXPECT_FALSE(CmpGreaterEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpLessEqualWillReturnTrueIfValueIsOneUlpLower)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::max())};
    const std::int32_t tolerance{0};
    EXPECT_TRUE(CmpLessEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpLessEqualWillReturnTrueIfEqual)
{
    const double lhs{1.0};
    const double rhs{lhs};
    const std::int32_t tolerance{0};
    EXPECT_TRUE(CmpLessEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpLessEqualWillReturnTrueIfWithinTolerance)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::lowest())};
    const std::int32_t tolerance{1};
    EXPECT_TRUE(CmpLessEqual(lhs, rhs, tolerance));
}

TEST(FloatingTypesSpecialTests, CmpLessEqualWillReturnFalseIfOutsideTolerance)
{
    const double lhs{1.0};
    const double rhs{score::cpp::nextafter(lhs, std::numeric_limits<double>::lowest())};
    const std::int32_t tolerance{0};
    EXPECT_FALSE(CmpLessEqual(lhs, rhs, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsIntegralFloatInRangeRhs32WillReturnTrueIfConditionTrue)
{
    const double lhs{1.0};
    const std::int32_t rhs{2};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhs32RhsIntegralFloatInRangeWillReturnTrueIfConditionTrue)
{
    const std::int32_t lhs{1};
    const double rhs{2.0};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsIntegralFloatInRangeRhs32WillReturnFalseIfConditionFalse)
{
    const double lhs{1.0};
    const std::int32_t rhs{1};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhs32RhsIntegralFloatInRangeWillReturnFalseIfConditionFalse)
{
    const std::int32_t lhs{1};
    const double rhs{1.0};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsPositiveFractionalFloatInRangeRhs32WillReturnTrueIfConditionTrue)
{
    const double lhs{0.9};
    const std::int32_t rhs{1};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhs32RhsPositiveFractionalFloatInRangeWillReturnTrueIfConditionTrue)
{
    const std::int32_t lhs{1};
    const double rhs{1.1};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsPositiveFractionalFloatInRangeRhs32WillReturnFalseIfConditionFalse)
{
    const double lhs{1.1};
    const std::int32_t rhs{1};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhs32RhsPositiveFractionalFloatInRangeWillReturnFalseIfConditionFalse)
{
    const std::int32_t lhs{1};
    const double rhs{0.9};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsNegativeFractionalFloatInRangeRhs32WillReturnTrueIfConditionTrue)
{
    const double lhs{-1.1};
    const std::int32_t rhs{-1};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhs32RhsNegativeFractionalFloatInRangeWillReturnTrueIfConditionTrue)
{
    const std::int32_t lhs{-1};
    const double rhs{-0.9};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsNegativeFractionalFloatInRangeRhs32WillReturnFalseIfConditionFalse)
{
    const double lhs{-0.9};
    const std::int32_t rhs{-1};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhs32RhsNegativeFractionalFloatInRangeWillReturnFalseIfConditionFalse)
{
    const std::int32_t lhs{-1};
    const double rhs{-1.1};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsU64RhsIntegralFloatInRangeWillReturnTrueIfConditionTrue)
{
    const std::uint64_t lhs{1U};
    const double rhs{2.0};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsIntegralFloatInRangeRhsU64WillReturnTrueIfConditionTrue)
{
    const double lhs{1.0};
    const std::uint64_t rhs{2U};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsU64RhsIntegralFloatInRangeWillReturnFalseIfConditionFalse)
{
    const std::uint64_t lhs{1U};
    const double rhs{1.0};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsIntegralFloatInRangeRhsU64WillReturnFalseIfConditionFalse)
{
    const double lhs{1.0};
    const std::uint64_t rhs{1U};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsU64RhsFloatBelowRangeWillReturnFalse)
{
    const auto lhs{std::numeric_limits<std::uint64_t>::lowest()};
    const auto rhs{-std::numeric_limits<double>::min()};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsFloatBelowRangeRhsU64WillReturnTrue)
{
    const auto lhs{-std::numeric_limits<double>::min()};
    const auto rhs{std::numeric_limits<std::uint64_t>::lowest()};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsU64RhsFloatAboveRangeWillReturnTrue)
{
    const auto lhs{std::numeric_limits<std::uint64_t>::max()};
    const auto rhs{std::numeric_limits<double>::max()};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsFloatAboveRangeRhsU64WillReturnFalse)
{
    const auto lhs{std::numeric_limits<double>::max()};
    const auto rhs{std::numeric_limits<std::uint64_t>::max()};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhs64RhsFloatInRangeWillReturnTrueIfConditionTrue)
{
    const std::int64_t lhs{1};
    const double rhs{2.0};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsFloatInRangeRhs64WillReturnTrueIfConditionTrue)
{
    const double lhs{1.0};
    const std::int64_t rhs{2};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhs64RhsFloatInRangeWillReturnFalseIfConditionFalse)
{
    const std::int64_t lhs{1};
    const double rhs{1.0};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsFloatInRangeRhs64WillReturnFalseIfConditionFalse)
{
    const double lhs{1.0};
    const std::int64_t rhs{1};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhs64RhsFloatBelowRangeWillReturnFalse)
{
    const auto lhs{std::numeric_limits<std::int64_t>::lowest()};
    const auto rhs{std::numeric_limits<double>::lowest()};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsFloatBelowRangeRhs64WillReturnTrue)
{
    const auto lhs{std::numeric_limits<double>::lowest()};
    const auto rhs{std::numeric_limits<std::int64_t>::lowest()};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhs64RhsFloatAboveRangeWillReturnTrue)
{
    const auto lhs{std::numeric_limits<std::int64_t>::max()};
    const auto rhs{std::numeric_limits<double>::max()};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessLhsFloatAboveRangeRhs64WillReturnFalse)
{
    const auto lhs{std::numeric_limits<double>::max()};
    const auto rhs{std::numeric_limits<std::int64_t>::max()};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessNegativeLhsFloatWithUnsignedRhsWillReturnTrue)
{
    const auto lhs{-std::numeric_limits<double>::min()};
    const auto rhs{0U};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessUnsignedLhsWithNegativeRhsFloatWillReturnFalse)
{
    const auto lhs{0U};
    const auto rhs{-std::numeric_limits<double>::min()};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessNanLhsFloatWithUnsignedRhsWillReturnFalse)
{
    const auto lhs{std::numeric_limits<double>::quiet_NaN()};
    const auto rhs{0U};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessUnsignedLhsWithNanRhsFloatWillReturnFalse)
{
    const auto lhs{0U};
    const auto rhs{std::numeric_limits<double>::quiet_NaN()};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessNegativeInfinityLhsFloatWithUnsignedRhsWillReturnTrue)
{
    const auto lhs{-std::numeric_limits<double>::infinity()};
    const auto rhs{0U};
    EXPECT_TRUE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessInfinityLhsFloatWithUnsignedRhsWillReturnFalse)
{
    const auto lhs{std::numeric_limits<double>::infinity()};
    const auto rhs{0U};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessUnsignedLhsWithNegativeInfinityRhsFloatWillReturnFalse)
{
    const auto lhs{0U};
    const auto rhs{-std::numeric_limits<double>::infinity()};
    EXPECT_FALSE(CmpLess(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpLessUnsignedLhsWithPositiveInfinityRhsFloatWillReturnTrue)
{
    const auto lhs{std::numeric_limits<double>::infinity()};
    const std::uint8_t rhs{std::numeric_limits<std::uint8_t>::max()};
    EXPECT_TRUE(CmpGreater(lhs, rhs));
}

TEST(FloatingIntegralTypesTests, CmpEqualLhsFloatRhs64WillCheckPerfectEqualityByDefault)
{
    const std::int32_t tolerance{0};
    const double lhs_equal{1.0};
    const std::int64_t rhs{1};
    EXPECT_TRUE(CmpEqual(lhs_equal, rhs, tolerance));

    const auto lhs_unequal{score::cpp::nextafter(lhs_equal, 0.0)};
    EXPECT_FALSE(CmpEqual(lhs_unequal, rhs, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpEqualLhsFloatRhs64WithOneUlpBelowWillSucceedWithValuesInBounds)
{
    const std::int64_t rhs{1};
    const auto lhs_unequal{score::cpp::nextafter(static_cast<double>(rhs), 0.0)};
    const std::int32_t tolerance{1};
    EXPECT_TRUE(CmpEqual(lhs_unequal, rhs, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpEqualLhsFloatRhs64WithOneUlpAboveWillSucceedWithValuesInBounds)
{
    const std::int64_t rhs{1};
    const auto lhs_unequal{score::cpp::nextafter(static_cast<double>(rhs), 2.0)};
    const std::int32_t tolerance{1};
    EXPECT_TRUE(CmpEqual(lhs_unequal, rhs, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpEqualLhs64RhsFloatForwardsCorrectlyToCmpEqualLhsFloatRhsInt)
{
    const std::int32_t tolerance{0};
    const std::int64_t lhs{1};
    const double rhs_equal{1.0};
    EXPECT_TRUE(CmpEqual(lhs, rhs_equal, tolerance));

    const auto rhs_unequal{score::cpp::nextafter(rhs_equal, 0.0)};
    EXPECT_FALSE(CmpEqual(lhs, rhs_unequal, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpEqualReturnsFalseWhenComparedWithNan)
{
    const std::int32_t tolerance{0};
    const auto lhs{std::numeric_limits<double>::quiet_NaN()};
    const std::int64_t rhs{1};
    EXPECT_FALSE(CmpEqual(lhs, rhs, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpNotEqualLhsFloatRhsIntForwardsCorrectlyToCmpEqual)
{
    std::int32_t tolerance{0};
    const double lhs_equal{1.0};
    const std::int64_t rhs{1};
    EXPECT_FALSE(CmpNotEqual(lhs_equal, rhs, tolerance));

    const auto lhs_unequal{score::cpp::nextafter(lhs_equal, 0.0)};
    EXPECT_TRUE(CmpNotEqual(lhs_unequal, rhs, tolerance));

    tolerance = 1;
    EXPECT_FALSE(CmpNotEqual(lhs_unequal, rhs, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpNotEqualLhsIntRhsFloatForwardsCorrectlyToCmpEqual)
{
    std::int32_t tolerance{0};
    const std::int64_t lhs{1};
    const double rhs_equal{1.0};
    EXPECT_FALSE(CmpNotEqual(lhs, rhs_equal, tolerance));

    const auto rhs_unequal{score::cpp::nextafter(rhs_equal, 0.0)};
    EXPECT_TRUE(CmpNotEqual(lhs, rhs_unequal, tolerance));

    tolerance = 1;
    EXPECT_FALSE(CmpNotEqual(lhs, rhs_unequal, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpGreaterLhsFloatRhsIntForwardsCorrectlyToCmpLess)
{
    const double lhs_unequal{1.1};
    const std::int64_t rhs{1};
    EXPECT_TRUE(CmpGreater(lhs_unequal, rhs));

    const double lhs_equal{1.0};
    EXPECT_FALSE(CmpGreater(lhs_equal, rhs));
}

TEST(FloatingIntegralTypesTests, CmpGreaterLhsIntRhsFloatForwardsCorrectlyToCmpLess)
{
    const std::int64_t lhs{1};
    const double rhs_unequal{0.9};
    EXPECT_TRUE(CmpGreater(lhs, rhs_unequal));

    const double rhs_equal{1.0};
    EXPECT_FALSE(CmpGreater(lhs, rhs_equal));
}

TEST(FloatingIntegralTypesTests, CmpLessEqualLhsFloatRhs64RForwardsCorrectlyToCmpEqualAndCmpLess)
{
    std::int32_t tolerance{0};
    const double lhs_equal{1.0};
    const std::int64_t rhs{1};
    EXPECT_TRUE(CmpLessEqual(lhs_equal, rhs, tolerance));

    const double lhs_higher{score::cpp::nextafter(lhs_equal, 2.0)};
    EXPECT_FALSE(CmpLessEqual(lhs_higher, rhs, tolerance));

    const auto lhs_lower{score::cpp::nextafter(lhs_equal, 0.0)};
    EXPECT_TRUE(CmpLessEqual(lhs_lower, rhs, tolerance));

    tolerance = 1;
    EXPECT_TRUE(CmpLessEqual(lhs_higher, rhs, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpLessEqualLhs64RhsFloatForwardsCorrectlyToCmpEqualAndCmpLess)
{
    std::int32_t tolerance{0};
    const std::int64_t lhs{1};
    const double rhs_equal{1.0};
    EXPECT_TRUE(CmpLessEqual(lhs, rhs_equal, tolerance));

    const double rhs_higher{score::cpp::nextafter(rhs_equal, 2.0)};
    EXPECT_TRUE(CmpLessEqual(lhs, rhs_higher, tolerance));

    const auto rhs_lower{score::cpp::nextafter(rhs_equal, 0.0)};
    EXPECT_FALSE(CmpLessEqual(lhs, rhs_lower, tolerance));

    tolerance = 1;
    EXPECT_TRUE(CmpLessEqual(lhs, rhs_lower, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpGreaterEqualLhsFloatRhs64RForwardsCorrectlyToCmpEqualAndCmpGreater)
{
    std::int32_t tolerance{0};
    const double lhs_equal{1.0};
    const std::int64_t rhs{1};
    EXPECT_TRUE(CmpGreaterEqual(lhs_equal, rhs, tolerance));

    const double lhs_higher{score::cpp::nextafter(lhs_equal, 2.0)};
    EXPECT_TRUE(CmpGreaterEqual(lhs_higher, rhs, tolerance));

    const auto lhs_lower{score::cpp::nextafter(lhs_equal, 0.0)};
    EXPECT_FALSE(CmpGreaterEqual(lhs_lower, rhs, tolerance));

    tolerance = 1;
    EXPECT_TRUE(CmpGreaterEqual(lhs_lower, rhs, tolerance));
}

TEST(FloatingIntegralTypesTests, CmpGreaterEqualLhs64RhsFloatForwardsCorrectlyToCmpEqualAndCmpGreater)
{
    std::int32_t tolerance{0};
    const std::int64_t lhs{1};
    const double rhs_equal{1.0};
    EXPECT_TRUE(CmpGreaterEqual(lhs, rhs_equal, tolerance));

    const double rhs_higher{score::cpp::nextafter(rhs_equal, 2.0)};
    EXPECT_FALSE(CmpGreaterEqual(lhs, rhs_higher, tolerance));

    const auto rhs_lower{score::cpp::nextafter(rhs_equal, 0.0)};
    EXPECT_TRUE(CmpGreaterEqual(lhs, rhs_lower, tolerance));

    tolerance = 1;
    EXPECT_TRUE(CmpGreaterEqual(lhs, rhs_higher, tolerance));
}

}  // namespace
}  // namespace score::safe_math
