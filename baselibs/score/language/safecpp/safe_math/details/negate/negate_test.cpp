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
#include "score/language/safecpp/safe_math/details/negate/negate.h"
#include "score/language/safecpp/safe_math/details/return_mode_test_helper.h"
#include "score/language/safecpp/safe_math/details/test_type_collection.h"
#include "score/language/safecpp/safe_math/return_mode.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <type_traits>

namespace score::safe_math
{
namespace
{

// Unified test fixture that works for both ReturnModes
template <typename TypeAndMode>
class NegateTest : public ::testing::Test
{
};

// Define test types for all combinations of IntegerType x ReturnMode
using NegateTestTypes = ::testing::Types<TypeWithMode<std::uint8_t, ReturnMode::kReturnResultOnError>,
                                         TypeWithMode<std::uint16_t, ReturnMode::kReturnResultOnError>,
                                         TypeWithMode<std::uint32_t, ReturnMode::kReturnResultOnError>,
                                         TypeWithMode<std::uint64_t, ReturnMode::kReturnResultOnError>,
                                         TypeWithMode<std::int8_t, ReturnMode::kReturnResultOnError>,
                                         TypeWithMode<std::int16_t, ReturnMode::kReturnResultOnError>,
                                         TypeWithMode<std::int32_t, ReturnMode::kReturnResultOnError>,
                                         TypeWithMode<std::int64_t, ReturnMode::kReturnResultOnError>,
                                         TypeWithMode<std::uint8_t, ReturnMode::kAbortOnError>,
                                         TypeWithMode<std::uint16_t, ReturnMode::kAbortOnError>,
                                         TypeWithMode<std::uint32_t, ReturnMode::kAbortOnError>,
                                         TypeWithMode<std::uint64_t, ReturnMode::kAbortOnError>,
                                         TypeWithMode<std::int8_t, ReturnMode::kAbortOnError>,
                                         TypeWithMode<std::int16_t, ReturnMode::kAbortOnError>,
                                         TypeWithMode<std::int32_t, ReturnMode::kAbortOnError>,
                                         TypeWithMode<std::int64_t, ReturnMode::kAbortOnError>>;

TYPED_TEST_SUITE(NegateTest, NegateTestTypes, /* unused */);

TYPED_TEST(NegateTest, CanNegateNegativeValue)
{
    using T = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    if (std::is_unsigned<T>::value)
    {
        GTEST_SKIP() << "Test not supported for unsigned types";
    }
    const auto value_to_be_negated{static_cast<typename std::make_signed_t<T>>(-1)};
    const auto result = Negate<kMode>(value_to_be_negated);
    Helper::ExpectHasValue(result);
    EXPECT_TRUE(CmpEqual(Helper::GetValue(result), Abs(value_to_be_negated)));
}

TYPED_TEST(NegateTest, CanNegatePositiveValue)
{
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto value_to_be_negated{1U};
    const auto result = Negate<kMode>(value_to_be_negated);
    Helper::ExpectSuccess(result, -1);
}

TYPED_TEST(NegateTest, CanNegatePositiveToMinimum)
{
    using T = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto signed_max = std::numeric_limits<typename std::make_signed_t<T>>::max();
    const auto value_to_be_negated{static_cast<typename std::make_unsigned_t<T>>(signed_max) + 1U};
    const auto result = Negate<kMode, typename std::make_signed_t<T>>(value_to_be_negated);
    Helper::ExpectSuccess(result, std::numeric_limits<typename std::make_signed_t<T>>::min());
}

TYPED_TEST(NegateTest, NegateWillThrowErrorWhenResultNotRepresentable)
{
    using T = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto signed_max = std::numeric_limits<typename std::make_signed_t<T>>::max();
    const auto value_to_be_negated{static_cast<typename std::make_unsigned_t<T>>(signed_max) + 2U};

    Helper::ExpectErrorFromOperation(
        [&]() {
            return Negate<kMode, typename std::make_signed_t<T>>(value_to_be_negated);
        },
        ErrorCode::kExceedsNumericLimits);
}

// Non-templated tests for specific scenarios (tested with both ReturnModes)
using ReturnModes = ::testing::Types<std::integral_constant<ReturnMode, ReturnMode::kReturnResultOnError>,
                                     std::integral_constant<ReturnMode, ReturnMode::kAbortOnError>>;

template <typename ModeConstant>
class NegateMinimumTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(NegateMinimumTest, ReturnModes, /* unused */);

TYPED_TEST(NegateMinimumTest, CanNegateLowestValue)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto value_to_be_negated{std::numeric_limits<std::int32_t>::lowest()};
    const auto result = Negate<kMode, std::uint32_t>(value_to_be_negated);
    Helper::ExpectSuccess(result, Abs(value_to_be_negated));
}

// Floating-point tests (templated for both modes)
template <typename ModeConstant>
class NegateFloatingPointsTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(NegateFloatingPointsTest, ReturnModes, /* unused */);

TYPED_TEST(NegateFloatingPointsTest, CanNegatePositiveNumber)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double value{6.51};
    const auto result = Negate<kMode>(value);
    Helper::ExpectSuccess(result, -value);
}

TYPED_TEST(NegateFloatingPointsTest, CanNegateNegativeNumber)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double value{-6.51};
    const auto result = Negate<kMode>(value);
    Helper::ExpectSuccess(result, -value);
}

TYPED_TEST(NegateFloatingPointsTest, NegateWillReturnErrorWhenResultNotRepresentable)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto value_to_be_negated = std::numeric_limits<double>::max();

    Helper::ExpectErrorFromOperation(
        [&]() {
            return Negate<kMode, float>(value_to_be_negated);
        },
        ErrorCode::kExceedsNumericLimits);
}

TEST(NegateTypefirstConvenienceOverloadTest, NegateWithExplicitReturnTypeUsesDefaultReturnMode)
{
    using Helper = ReturnModeTestHelper<ReturnMode::kReturnResultOnError>;

    const std::int32_t value{5};
    const auto result = Negate<std::int32_t>(value);
    Helper::ExpectSuccess(result, std::int32_t{-5});
}

TEST(NegateTypefirstConvenienceOverloadTest, NegateOverflowWithExplicitReturnTypeReturnsError)
{
    using Helper = ReturnModeTestHelper<ReturnMode::kReturnResultOnError>;

    const std::uint32_t value{static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()) + 2U};
    const auto result = Negate<std::int32_t>(value);
    Helper::ExpectError(result, ErrorCode::kExceedsNumericLimits);
}

}  // namespace
}  // namespace score::safe_math
