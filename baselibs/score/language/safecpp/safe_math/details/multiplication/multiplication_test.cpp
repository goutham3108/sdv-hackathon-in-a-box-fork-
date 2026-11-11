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
#include "score/language/safecpp/safe_math/details/multiplication/multiplication.h"
#include "score/language/safecpp/safe_math/details/return_mode_test_helper.h"
#include "score/language/safecpp/safe_math/details/test_type_collection.h"
#include "score/language/safecpp/safe_math/return_mode.h"

#include <gtest/gtest.h>

#include <limits>
#include <type_traits>

namespace score::safe_math
{
namespace
{

template <typename TypeAndMode>
class MultiplicationTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(MultiplicationTest, WithBothModes<UnsignedTypes>::type, /* unused */);

TYPED_TEST(MultiplicationTest, MultiplicationWithUnsignedAndUnsignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{2U};
    const Type val2{3U};
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), static_cast<Type>(val1 * val2));
}

TYPED_TEST(MultiplicationTest, MultiplicationWithUnsignedAndUnsignedMaxWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{1U};
    const Type val2{std::numeric_limits<Type>::max()};
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), static_cast<Type>(val1 * val2));
}

TYPED_TEST(MultiplicationTest, MultiplicationWithPositiveSignedAndPositiveSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{2};
    const typename std::make_signed_t<Type> val2{3};
    using SignedType = typename std::make_signed_t<Type>;
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), static_cast<SignedType>(val1 * val2));
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndNegativeSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{-2};
    const typename std::make_signed_t<Type> val2{-3};
    using SignedType = typename std::make_signed_t<Type>;
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), static_cast<SignedType>(val1 * val2));
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndNegativeSignedLowestWithoutOverflowWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{-1};
    const auto val2{std::numeric_limits<typename std::make_signed_t<Type>>::lowest() + 1};
    using SignedType = typename std::make_signed_t<Type>;
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), static_cast<SignedType>(val1 * val2));
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndNegativeSignedAndOverflowThrowsError)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{-1};
    const auto val2{std::numeric_limits<typename std::make_signed_t<Type>>::lowest()};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Multiply<kMode>(val1, val2);
        },
        ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndPositiveSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{-2};
    const typename std::make_signed_t<Type> val2{3};
    using SignedType = typename std::make_signed_t<Type>;
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), static_cast<SignedType>(val1 * val2));
}

TYPED_TEST(MultiplicationTest, MultiplicationWithPositiveSignedAndLowestSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{1};
    const auto val2{std::numeric_limits<typename std::make_signed_t<Type>>::lowest()};
    using SignedType = typename std::make_signed_t<Type>;
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), static_cast<SignedType>(val1 * val2));
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndMaxSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{-1};
    const auto val2{std::numeric_limits<typename std::make_signed_t<Type>>::max()};
    using SignedType = typename std::make_signed_t<Type>;
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), static_cast<SignedType>(val1 * val2));
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndFirstUnsupportedNegativeSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{-2};
    const auto val2{-((std::numeric_limits<typename std::make_signed_t<Type>>::max() / 2) + 1)};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Multiply<kMode>(val1, val2);
        },
        ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithPositiveSignedAndNegativeSignedAndUnderflowThrowsError)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{3};
    const auto val2{(std::numeric_limits<typename std::make_signed_t<Type>>::lowest() / val1) - 1};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Multiply<kMode>(val1, val2);
        },
        ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndUnsignedExceedingLimitsThrowsError)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{-1};
    const auto max = std::numeric_limits<typename std::make_signed_t<Type>>::max();
    const Type val2{static_cast<Type>(max) + 2U};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Multiply<kMode>(val1, val2);
        },
        ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(MultiplicationTest, MultiplicationWithNegativeSignedAndUnsignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{-1};
    const auto max = std::numeric_limits<typename std::make_signed_t<Type>>::max();
    const Type val2{max};
    using SignedType = typename std::make_signed_t<Type>;
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), static_cast<SignedType>(-static_cast<SignedType>(max)));
}

TYPED_TEST(MultiplicationTest, MultiplicationWithPositiveSignedAndUnsignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{1};
    const Type val2{std::numeric_limits<typename std::make_signed_t<Type>>::max()};
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), std::numeric_limits<typename std::make_signed_t<Type>>::max());
}

TYPED_TEST(MultiplicationTest, MultiplicationWithUnsignedAndPositiveSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto val1{std::numeric_limits<Type>::max()};
    const typename std::make_signed_t<Type> val2{1};
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), std::numeric_limits<Type>::max());
}

TYPED_TEST(MultiplicationTest, MultiplicationByZeroAsFirstArgumentReturnsZero)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{0};
    const Type val2{1};
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), Type{0});
}

TYPED_TEST(MultiplicationTest, MultiplicationByZeroAsSecondArgumentReturnsZero)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{1};
    const Type val2{0};
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), Type{0});
}

template <typename TypeAndMode>
class MultiplicationMultipleTypesTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(MultiplicationMultipleTypesTest);

TYPED_TEST_P(MultiplicationMultipleTypesTest, MultiplicationWorks)
{
    using TypePair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto max{std::numeric_limits<typename TypePair::first_type>::max()};
    const typename TypePair::first_type val1{max / static_cast<typename TypePair::first_type>(2)};
    const typename TypePair::second_type val2{2};
    using ExpectedType = typename TypePair::first_type;
    Helper::ExpectSuccess(Multiply<kMode>(val1, val2), static_cast<ExpectedType>(val1 * val2));
}

TYPED_TEST_P(MultiplicationMultipleTypesTest, MultiplicationExceedingLimitsReturnsError)
{
    using TypePair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto max{std::numeric_limits<typename TypePair::first_type>::max()};
    const auto half_max{(max / static_cast<typename TypePair::first_type>(2))};
    const typename TypePair::first_type val1{half_max + static_cast<typename TypePair::first_type>(1)};
    const typename TypePair::second_type val2{2};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Multiply<kMode>(val1, val2);
        },
        ErrorCode::kExceedsNumericLimits);
}

REGISTER_TYPED_TEST_SUITE_P(MultiplicationMultipleTypesTest,
                            MultiplicationWorks,
                            MultiplicationExceedingLimitsReturnsError);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypes,
                               MultiplicationMultipleTypesTest,
                               WithBothModesPairs<UnsignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypes, MultiplicationMultipleTypesTest, WithBothModesPairs<SignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedToUnsignedTypes,
                               MultiplicationMultipleTypesTest,
                               WithBothModesPairs<SignedUnsignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedToSignedTypes,
                               MultiplicationMultipleTypesTest,
                               WithBothModesPairs<UnsignedSignedTypePairs>::type,
                               /* unused */);

// Floating-point tests (templated for both modes)
template <typename ModeConstant>
class MultiplicationFloatingPointsTest : public ::testing::Test
{
};

using ReturnModes = ::testing::Types<std::integral_constant<ReturnMode, ReturnMode::kReturnResultOnError>,
                                     std::integral_constant<ReturnMode, ReturnMode::kAbortOnError>>;

TYPED_TEST_SUITE(MultiplicationFloatingPointsTest, ReturnModes, /* unused */);

TYPED_TEST(MultiplicationFloatingPointsTest, CanMultiplyTwoFloats)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{3.1};
    const double rhs{2.1};
    const auto result = Multiply<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, lhs * rhs);
}

TYPED_TEST(MultiplicationFloatingPointsTest, CanMultiplyFloatWithInteger)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{3.1};
    const std::uint32_t rhs{2U};
    const auto result = Multiply<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, 6.2);
}

TYPED_TEST(MultiplicationFloatingPointsTest, MultiplicationFailsIfIntegerCanNotBeRepresentedInFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{3.1};
    const auto rhs{std::numeric_limits<std::uint64_t>::max()};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Multiply<kMode>(lhs, rhs);
        },
        ErrorCode::kImplicitRounding);
}

TYPED_TEST(MultiplicationFloatingPointsTest, CanMultiplyIntegerWithFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const std::uint32_t lhs{2U};
    const double rhs{3.1};
    const auto result = Multiply<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, 6.2);
}

TEST(MultiplyTypefirstConvenienceOverloadTest, MultiplyWithExplicitReturnTypeUsesDefaultReturnMode)
{
    using Helper = ReturnModeTestHelper<ReturnMode::kReturnResultOnError>;

    const std::int32_t lhs{3};
    const std::int32_t rhs{4};
    const auto result = Multiply<std::int64_t>(lhs, rhs);
    Helper::ExpectSuccess(result, std::int64_t{12});
}

TEST(MultiplyTypefirstConvenienceOverloadTest, MultiplyOverflowWithExplicitReturnTypeReturnsError)
{
    using Helper = ReturnModeTestHelper<ReturnMode::kReturnResultOnError>;

    const std::int32_t lhs{std::numeric_limits<std::int32_t>::max()};
    const std::int32_t rhs{2};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Multiply<std::int32_t>(lhs, rhs);
        },
        ErrorCode::kExceedsNumericLimits);
}

}  // namespace
}  // namespace score::safe_math
