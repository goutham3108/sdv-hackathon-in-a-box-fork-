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
#include "score/language/safecpp/safe_math/details/cast/cast.h"
#include "score/language/safecpp/safe_math/details/return_mode_test_helper.h"
#include "score/language/safecpp/safe_math/details/test_type_collection.h"
#include "score/language/safecpp/safe_math/return_mode.h"

#include <gtest/gtest.h>

#include <cmath>

namespace score::safe_math
{
namespace
{
template <typename TypeAndMode>
class CastTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(CastTest);

TYPED_TEST_P(CastTest, CastMinimumWorks)
{
    using TypePair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto min_1 = std::numeric_limits<typename TypePair::first_type>::lowest();
    const auto min_2 = std::numeric_limits<typename TypePair::second_type>::lowest();

    typename TypePair::first_type value_to_be_cast{};
    if (CmpGreaterEqual(min_1, min_2))
    {
        value_to_be_cast = min_1;
    }
    else
    {
        value_to_be_cast = static_cast<typename TypePair::first_type>(min_2);
    }

    Helper::ExpectSuccess(Cast<kMode, typename TypePair::second_type>(value_to_be_cast),
                          static_cast<typename TypePair::second_type>(value_to_be_cast));
}

TYPED_TEST_P(CastTest, CastMaximumWorks)
{
    using TypePair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto max_1 = std::numeric_limits<typename TypePair::first_type>::max();
    const auto max_2 = std::numeric_limits<typename TypePair::second_type>::max();

    typename TypePair::first_type value_to_be_cast{};
    if (CmpLessEqual(max_1, max_2))
    {
        value_to_be_cast = max_1;
    }
    else
    {
        value_to_be_cast = static_cast<typename TypePair::first_type>(max_2);
    }

    Helper::ExpectSuccess(Cast<kMode, typename TypePair::second_type>(value_to_be_cast),
                          static_cast<typename TypePair::second_type>(value_to_be_cast));
}

REGISTER_TYPED_TEST_SUITE_P(CastTest, CastMinimumWorks, CastMaximumWorks);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypes, CastTest, WithBothModesPairs<UnsignedTypePairs>::type, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypes, CastTest, WithBothModesPairs<SignedTypePairs>::type, /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedUnsignedTypes, CastTest, WithBothModesPairs<SignedUnsignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedSignedTypes, CastTest, WithBothModesPairs<UnsignedSignedTypePairs>::type,
                               /* unused */);

TEST(CastErrorMinimumExceededTest, CastMinimumLimitExceededReturnsError)
{
    const auto min = std::numeric_limits<std::int16_t>::lowest();
    const auto value_to_be_cast = static_cast<std::int32_t>(min) - 1;

    const auto result = Cast<std::int16_t>(value_to_be_cast);
    ASSERT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(CastErrorMaximumExceededTest, CastMaximumLimitExceededReturnsError)
{
    const auto max = std::numeric_limits<std::int16_t>::max();
    const auto value_to_be_cast = static_cast<std::int32_t>(max) + 1;

    const auto result = Cast<std::int16_t>(value_to_be_cast);
    ASSERT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

// Floating-to-Floating tests (templated for both modes)
using ReturnModes = ::testing::Types<std::integral_constant<ReturnMode, ReturnMode::kReturnResultOnError>,
                                     std::integral_constant<ReturnMode, ReturnMode::kAbortOnError>>;

template <typename ModeConstant>
class FloatingToFloatingTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(FloatingToFloatingTest, ReturnModes, /* unused */);

TYPED_TEST(FloatingToFloatingTest, CanCastNan)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto result = Cast<kMode, double>(nan);
    Helper::ExpectHasValue(result);
    EXPECT_TRUE(std::isnan(Helper::GetValue(result)));
}

TYPED_TEST(FloatingToFloatingTest, CanCastInfinity)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto infinity = std::numeric_limits<double>::infinity();
    const auto result = Cast<kMode, double>(infinity);
    Helper::ExpectHasValue(result);
    EXPECT_TRUE(std::isinf(Helper::GetValue(result)));
}

TYPED_TEST(FloatingToFloatingTest, CanCastNegativeInfinity)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto infinity = -std::numeric_limits<double>::infinity();
    const auto result = Cast<kMode, double>(infinity);
    Helper::ExpectHasValue(result);
    EXPECT_TRUE(std::signbit(Helper::GetValue(result)));
    EXPECT_TRUE(std::isinf(Helper::GetValue(result)));
}

TYPED_TEST(FloatingToFloatingTest, CanCastFromDoubleToFloatWhenNotLoosingPrecision)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto min_float = std::numeric_limits<float>::min();
    const auto result = Cast<kMode, float>(static_cast<double>(min_float));
    Helper::ExpectSuccess(result, min_float);
}

TEST(FloatingToFloatingErrorTest, ExceedingMaximumOfResultTypeWillReturnError)
{
    const auto max_float = static_cast<double>(std::numeric_limits<float>::max());
    const auto exceeding_float = std::nextafter(max_float, std::numeric_limits<double>::max());
    const auto result = Cast<float>(exceeding_float);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToFloatingErrorTest, ExceedingMinimumOfResultTypeWillReturnError)
{
    const auto min_float = static_cast<double>(std::numeric_limits<float>::lowest());
    const auto exceeding_float = std::nextafter(min_float, std::numeric_limits<double>::lowest());
    const auto result = Cast<float>(exceeding_float);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToFloatingErrorTest, CastFromDoubleToFloatWillReturnErrorWhenLoosingPrecision)
{
    const auto min_float = std::numeric_limits<float>::min();
    const auto exceeding_float = std::nextafter(static_cast<double>(min_float), std::numeric_limits<double>::lowest());
    const auto result = Cast<float>(exceeding_float);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

// FloatingToIntegral tests (templated for both modes)
template <typename ModeConstant>
class FloatingToIntegralTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(FloatingToIntegralTest, ReturnModes, /* unused */);

TYPED_TEST(FloatingToIntegralTest, CanCastIntegralValue)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double to_be_casted{3.0};
    const auto result = Cast<kMode, std::uint8_t>(to_be_casted);
    Helper::ExpectSuccess(result, std::uint8_t{3U});
}

TEST(FloatingToIntegralErrorTest, CastExceedingMinimumWillReturnError)
{
    const double to_be_casted{-std::numeric_limits<double>::min()};
    const auto result = Cast<std::uint8_t>(to_be_casted);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToIntegralErrorTest, CastExceedingMaximumWillReturnError)
{
    const double to_be_casted{static_cast<double>(std::numeric_limits<std::uint8_t>::max()) + 1.0};
    const auto result = Cast<std::uint8_t>(to_be_casted);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToIntegralErrorTest, CastingNonIntegralValueReturnsError)
{
    const double integral_float{3.0};
    const auto non_integral_float = std::nextafter(integral_float, std::numeric_limits<double>::lowest());
    const auto result = Cast<std::uint8_t>(non_integral_float);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

TEST(FloatingToIntegralErrorTest, CastingNanReturnsError)
{
    const auto nan{std::numeric_limits<double>::quiet_NaN()};
    const auto result = Cast<std::uint8_t>(nan);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToIntegralErrorTest, CastingPositiveInfinityReturnsError)
{
    const auto infinity{std::numeric_limits<double>::infinity()};
    const auto result = Cast<std::uint8_t>(infinity);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TEST(FloatingToIntegralErrorTest, CastingNegativeInfinityReturnsError)
{
    const auto infinity{-std::numeric_limits<double>::infinity()};
    const auto result = Cast<std::uint8_t>(infinity);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

// IntegralToFloating tests (templated for both modes)
template <typename ModeConstant>
class IntegralToFloatingTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(IntegralToFloatingTest, ReturnModes, /* unused */);

TYPED_TEST(IntegralToFloatingTest, CastingRepresentableValueReturnsCastedValue)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto representable_value{1U};
    const auto result = Cast<kMode, double>(representable_value);
    Helper::ExpectSuccess(result, 1.0);
}

TEST(IntegralToFloatingErrorTest, CastingUnrepresentableValueReturnsError)
{
    const auto unrepresentable_value{std::numeric_limits<std::uint64_t>::max()};
    const auto result = Cast<double>(unrepresentable_value);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ErrorCode::kImplicitRounding);
}

}  // namespace
}  // namespace score::safe_math
