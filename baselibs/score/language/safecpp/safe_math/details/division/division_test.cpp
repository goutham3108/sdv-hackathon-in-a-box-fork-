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
#include "score/language/safecpp/safe_math/details/division/division.h"
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
template <typename TypePairAndMode>
class DivisionMultipleTypesTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(DivisionMultipleTypesTest);

TYPED_TEST_P(DivisionMultipleTypesTest, DivisionWorks)
{
    using TPair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto max{std::numeric_limits<typename TPair::first_type>::max()};
    const typename TPair::first_type val1{max - static_cast<typename TPair::first_type>(1)};
    const typename TPair::second_type val2{2};
    using ExpectedType = typename TPair::first_type;
    Helper::ExpectSuccess(Divide<kMode>(val1, val2), static_cast<ExpectedType>((max - 1) / 2));
}

TYPED_TEST_P(DivisionMultipleTypesTest, DivisionWithRoundingReturnsError)
{
    using TPair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename TPair::first_type val1{3};
    const typename TPair::second_type val2{2};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Divide<kMode>(val1, val2);
        },
        ErrorCode::kImplicitRounding);
}

TYPED_TEST_P(DivisionMultipleTypesTest, DivisionByZeroReturnsError)
{
    using TPair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename TPair::first_type val1{3};
    const typename TPair::second_type val2{0};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Divide<kMode>(val1, val2);
        },
        ErrorCode::kDivideByZero);
}

REGISTER_TYPED_TEST_SUITE_P(DivisionMultipleTypesTest,
                            DivisionWorks,
                            DivisionWithRoundingReturnsError,
                            DivisionByZeroReturnsError);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypes,
                               DivisionMultipleTypesTest,
                               typename WithBothModesPairs<UnsignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypes,
                               DivisionMultipleTypesTest,
                               typename WithBothModesPairs<SignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedUnsignedTypes,
                               DivisionMultipleTypesTest,
                               typename WithBothModesPairs<SignedUnsignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedSignedTypes,
                               DivisionMultipleTypesTest,
                               typename WithBothModesPairs<UnsignedSignedTypePairs>::type,
                               /* unused */);

template <typename TypePairAndMode>
class DivisionUnsignedSignedTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(DivisionUnsignedSignedTest);

TYPED_TEST_P(DivisionUnsignedSignedTest, DivisionWithPositiveSignWorks)
{
    using TPair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename TPair::first_type val1{6};
    const typename TPair::second_type val2{2};
    using ExpectedType = typename TPair::first_type;
    Helper::ExpectSuccess(Divide<kMode>(val1, val2), ExpectedType{3});
}

TYPED_TEST_P(DivisionUnsignedSignedTest, DivisionWithNegativeSignWorks)
{
    using TPair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename TPair::first_type val1{6};
    const typename TPair::second_type val2{-2};
    const auto result = Divide<kMode, std::int8_t>(val1, val2);
    Helper::ExpectSuccess(result, std::int8_t{-3});
}

REGISTER_TYPED_TEST_SUITE_P(DivisionUnsignedSignedTest, DivisionWithPositiveSignWorks, DivisionWithNegativeSignWorks);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedSignedTypes,
                               DivisionUnsignedSignedTest,
                               typename WithBothModesPairs<UnsignedSignedTypePairs>::type,
                               /* unused */);

template <typename TypePairAndMode>
class DivisionSignedUnsignedTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(DivisionSignedUnsignedTest);

TYPED_TEST_P(DivisionSignedUnsignedTest, DivisionWithPositiveSignWorks)
{
    using TPair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename TPair::first_type val1{6};
    const typename TPair::second_type val2{2};
    using ExpectedType = typename TPair::first_type;
    Helper::ExpectSuccess(Divide<kMode>(val1, val2), ExpectedType{3});
}

TYPED_TEST_P(DivisionSignedUnsignedTest, DivisionWithNegativeSignWorks)
{
    using TPair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename TPair::first_type val1{-6};
    const typename TPair::second_type val2{2};
    const auto result = Divide<kMode, std::int8_t>(val1, val2);
    Helper::ExpectSuccess(result, std::int8_t{-3});
}

REGISTER_TYPED_TEST_SUITE_P(DivisionSignedUnsignedTest, DivisionWithPositiveSignWorks, DivisionWithNegativeSignWorks);

INSTANTIATE_TYPED_TEST_SUITE_P(SignedUnsignedTypes,
                               DivisionSignedUnsignedTest,
                               typename WithBothModesPairs<SignedUnsignedTypePairs>::type,
                               /* unused */);

template <typename ModeConstant>
class DivisionFloatingPointsTest : public ::testing::Test
{
};

using ReturnModes = ::testing::Types<std::integral_constant<ReturnMode, ReturnMode::kReturnResultOnError>,
                                     std::integral_constant<ReturnMode, ReturnMode::kAbortOnError>>;

TYPED_TEST_SUITE(DivisionFloatingPointsTest, ReturnModes, /* unused */);

TYPED_TEST(DivisionFloatingPointsTest, CanDivideTwoFloats)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{6.51};
    const double rhs{2.1};
    const auto result = Divide<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, lhs / rhs);
}

TYPED_TEST(DivisionFloatingPointsTest, DivisionByZeroReturnsError)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{1.0};
    const auto rhs{0.0};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Divide<kMode>(lhs, rhs);
        },
        ErrorCode::kDivideByZero);
}

TYPED_TEST(DivisionFloatingPointsTest, CanDivideFloatWithInteger)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{3.1};
    const std::uint32_t rhs{2U};
    const auto result = Divide<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, 1.55);
}

TYPED_TEST(DivisionFloatingPointsTest, DivisionFailsIfRhsIntegerCanNotBeRepresentedInFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{3.1};
    const auto rhs{std::numeric_limits<std::uint64_t>::max()};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Divide<kMode>(lhs, rhs);
        },
        ErrorCode::kImplicitRounding);
}

TYPED_TEST(DivisionFloatingPointsTest, CanDivideIntegerWithFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const std::uint32_t lhs{2U};
    const double rhs{0.5};
    const auto result = Divide<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, 4.0);
}

TYPED_TEST(DivisionFloatingPointsTest, CanDivideIntegerWithVerySmallFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const std::uint32_t lhs{2U};
    const double rhs{0.0000000001};
    const auto result = Divide<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, 2.0e10);
}

TYPED_TEST(DivisionFloatingPointsTest, DivisionFailsIfLhsIntegerCanNotBeRepresentedInFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto lhs{std::numeric_limits<std::uint64_t>::max()};
    const double rhs{3.1};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Divide<kMode>(lhs, rhs);
        },
        ErrorCode::kImplicitRounding);
}

TEST(DivideTypefirstConvenienceOverloadTest, DivideWithExplicitReturnTypeUsesDefaultReturnMode)
{
    using Helper = ReturnModeTestHelper<ReturnMode::kReturnResultOnError>;

    const std::int32_t lhs{6};
    const std::int32_t rhs{2};
    const auto result = Divide<std::int64_t>(lhs, rhs);
    Helper::ExpectSuccess(result, std::int64_t{3});
}

TEST(DivideTypefirstConvenienceOverloadTest, DivideWithRoundingWithExplicitReturnTypeReturnsError)
{
    using Helper = ReturnModeTestHelper<ReturnMode::kReturnResultOnError>;

    const std::int32_t lhs{7};
    const std::int32_t rhs{2};
    const auto result = Divide<std::int32_t>(lhs, rhs);
    Helper::ExpectError(result, ErrorCode::kImplicitRounding);
}

}  // namespace
}  // namespace score::safe_math
