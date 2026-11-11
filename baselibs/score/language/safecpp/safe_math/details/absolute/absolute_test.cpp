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
#include "score/language/safecpp/safe_math/details/absolute/absolute.h"

#include <score/math.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

namespace score::safe_math
{
namespace
{

TEST(AbsTest, CanAbsMinimumValue)
{
    const auto val{std::numeric_limits<std::int64_t>::lowest()};
    // Also in the test we must work around the fact that the lowest value of a signed integer may not be put into abs
    const auto expected_val = static_cast<std::uint64_t>(score::cpp::abs(val + 1)) + 1;
    EXPECT_EQ(Abs(val), expected_val);
}

TEST(AbsTest, CanAbsMinimumValueWithSmallType)
{
    const auto val{std::numeric_limits<std::int8_t>::lowest()};
    // Also in the test we must work around the fact that the lowest value of a signed integer may not be put into abs
    const auto expected_val = static_cast<std::uint8_t>(score::cpp::abs(val + 1)) + 1;
    EXPECT_EQ(Abs(val), expected_val);
}

TEST(AbsTest, CanAbsNegativeValue)
{
    const std::int64_t val{-1};
    const auto expected_val = static_cast<std::uint64_t>(score::cpp::abs(val));
    EXPECT_EQ(Abs(val), expected_val);
}

TEST(AbsTest, CanAbsPositiveValue)
{
    const std::int64_t val{1};
    const auto expected_val = static_cast<std::uint64_t>(score::cpp::abs(val));
    EXPECT_EQ(Abs(val), expected_val);
}

TEST(AbsTest, CanAbsZero)
{
    const std::int64_t val{0};
    const std::uint64_t expected_val{0U};
    EXPECT_EQ(Abs(val), expected_val);
}

using FloatingTypes = ::testing::Types<float, double>;

template <class T>
class FloatAbsTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(FloatAbsTest, FloatingTypes, /* unused */);

TYPED_TEST(FloatAbsTest, CanAbsNegativeFloat)
{
    const TypeParam val{-1.0};
    const TypeParam expected_val{1.0};
    EXPECT_EQ(Abs(val), expected_val);
}

TYPED_TEST(FloatAbsTest, CanAbsLowestFloat)
{
    const TypeParam val{std::numeric_limits<TypeParam>::lowest()};
    const TypeParam expected_val{std::numeric_limits<TypeParam>::max()};
    EXPECT_EQ(Abs(val), expected_val);
}

TYPED_TEST(FloatAbsTest, CanAbsPositiveFloat)
{
    const TypeParam val{1.0};
    const TypeParam expected_val{1.0};
    EXPECT_EQ(Abs(val), expected_val);
}

TYPED_TEST(FloatAbsTest, CanAbsHighestFloat)
{
    const TypeParam val{std::numeric_limits<TypeParam>::max()};
    const TypeParam expected_val{std::numeric_limits<TypeParam>::max()};
    EXPECT_EQ(Abs(val), expected_val);
}

TYPED_TEST(FloatAbsTest, CanAbsNegativeZeroFloat)
{
    const TypeParam val{-0.0};
    const TypeParam expected_val{0.0};
    EXPECT_EQ(Abs(val), expected_val);
}

TYPED_TEST(FloatAbsTest, CanAbsPositiveZeroFloat)
{
    const TypeParam val{0.0};
    const TypeParam expected_val{0.0};
    EXPECT_EQ(Abs(val), expected_val);
}

TYPED_TEST(FloatAbsTest, CanAbsNegativeInfFloat)
{
    const TypeParam val{-std::numeric_limits<TypeParam>::infinity()};
    const TypeParam expected_val{std::numeric_limits<TypeParam>::infinity()};
    EXPECT_EQ(Abs(val), expected_val);
}

TYPED_TEST(FloatAbsTest, CanAbsPositiveInfFloat)
{
    const TypeParam val{std::numeric_limits<TypeParam>::infinity()};
    const TypeParam expected_val{std::numeric_limits<TypeParam>::infinity()};
    EXPECT_EQ(Abs(val), expected_val);
}

TYPED_TEST(FloatAbsTest, CanAbsNegativeSilentNaNFloat)
{
    const TypeParam val{-std::numeric_limits<TypeParam>::quiet_NaN()};
    EXPECT_TRUE(score::cpp::isnan(Abs(val)));
}

TYPED_TEST(FloatAbsTest, CanAbsPositiveSilentNaNFloat)
{
    const TypeParam val{std::numeric_limits<TypeParam>::quiet_NaN()};
    EXPECT_TRUE(score::cpp::isnan(Abs(val)));
}

TYPED_TEST(FloatAbsTest, CanAbsNegativeSignalingNaNFloat)
{
    const TypeParam val{-std::numeric_limits<TypeParam>::signaling_NaN()};
    EXPECT_TRUE(score::cpp::isnan(Abs(val)));
}

TYPED_TEST(FloatAbsTest, CanAbsPositiveSignalingNaNFloat)
{
    const TypeParam val{std::numeric_limits<TypeParam>::signaling_NaN()};
    EXPECT_TRUE(score::cpp::isnan(Abs(val)));
}

}  // namespace
}  // namespace score::safe_math
