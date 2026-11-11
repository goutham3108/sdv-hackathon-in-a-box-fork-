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
#include "score/language/safecpp/safe_math/details/addition_subtraction/addition_subtraction.h"
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
// Helper to extract value type from Result<T> or return T if it's a plain type
template <typename T>
struct ExtractValueType
{
    using type = T;
};

template <typename T>
struct ExtractValueType<Result<T>>
{
    using type = T;
};

template <typename T>
using ExtractValueType_t = typename ExtractValueType<T>::type;

template <typename TypeAndMode>
class AddTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(AddTest, WithBothModes<UnsignedTypes>::type, /* unused */);

TYPED_TEST(AddTest, AddingTwoUnsignedWithoutOverflowWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{1};
    const Type val2{2};
    Helper::ExpectSuccess(Add<kMode>(val1, val2), static_cast<Type>(val1 + val2));
}

TYPED_TEST(AddTest, AddingTwoUnsignedWithOverflowThrowsError)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{std::numeric_limits<Type>::max()};
    const Type val2{1};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Add<kMode>(val1, val2);
        },
        ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(AddTest, AddingTwoUnsignedWithTooSmallResultTypeThrowsError)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    if constexpr (std::is_same_v<Type, std::uint8_t>)
    {
        GTEST_SKIP() << "Skipping, because there is no smaller result type than the input type";
    }
    const Type val1{std::numeric_limits<Type>::max()};
    const Type val2{1};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Add<kMode, std::uint8_t>(val1, val2);
        },
        ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(AddTest, AddingUnsignedWithPositiveSignedWithoutOverflowWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{1};
    const typename std::make_signed_t<Type> val2{2};
    Helper::ExpectSuccess(Add<kMode>(val1, val2), static_cast<Type>(val1 + val2));
}

TYPED_TEST(AddTest, AddingUnsignedWithPositiveSignedWithOverflowReturnsError)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{std::numeric_limits<Type>::max()};
    const typename std::make_signed_t<Type> val2{1};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Add<kMode>(val1, val2);
        },
        ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(AddTest, AddingUnsignedWithNegativeSignedWithoutOverflowOrUnderflowWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{1};
    const typename std::make_signed_t<Type> val2{-2};
    Helper::ExpectSuccess(Add<kMode, typename std::make_signed_t<Type>>(val1, val2),
                          typename std::make_signed_t<Type>{-1});
}

TYPED_TEST(AddTest, AddingUnsignedWithNegativeSignedWithUnderflowReturnsError)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{std::numeric_limits<Type>::min()};
    const typename std::make_signed_t<Type> val2{-1};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Add<kMode, Type>(val1, val2);
        },
        ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(AddTest, AddingSignedWithUnsignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{1};
    const Type val2{2};
    using ExpectedType = ExtractValueType_t<decltype(Add<kMode>(val1, val2))>;
    Helper::ExpectSuccess(Add<kMode>(val1, val2), static_cast<ExpectedType>(3));
}

TYPED_TEST(AddTest, AddingPositiveSignedWithPositiveSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{1};
    const typename std::make_signed_t<Type> val2{2};
    Helper::ExpectSuccess(Add<kMode>(val1, val2), typename std::make_signed_t<Type>{3});
}

TYPED_TEST(AddTest, AddingSignedWithNegativeSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{1};
    const typename std::make_signed_t<Type> val2{-2};
    Helper::ExpectSuccess(Add<kMode>(val1, val2), typename std::make_signed_t<Type>{-1});
}

template <typename TypePairWithModeParam>
class AdditionMultipleTypesTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(AdditionMultipleTypesTest);

TYPED_TEST_P(AdditionMultipleTypesTest, AdditionWorks)
{
    using TypePair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename TypePair::first_type val1{2};
    const typename TypePair::second_type val2{3};
    using ExpectedType = ExtractValueType_t<decltype(Add<kMode>(val1, val2))>;
    Helper::ExpectSuccess(Add<kMode>(val1, val2), static_cast<ExpectedType>(5));
}

REGISTER_TYPED_TEST_SUITE_P(AdditionMultipleTypesTest, AdditionWorks);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypesReturnResult,
                               AdditionMultipleTypesTest,
                               WithBothModesPairs<UnsignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypesReturnResult,
                               AdditionMultipleTypesTest,
                               WithBothModesPairs<SignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedToUnsignedTypesReturnResult,
                               AdditionMultipleTypesTest,
                               WithBothModesPairs<SignedUnsignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedToSignedTypesReturnResult,
                               AdditionMultipleTypesTest,
                               WithBothModesPairs<UnsignedSignedTypePairs>::type,
                               /* unused */);

// Floating-point tests (templated for both modes)
using ReturnModes = ::testing::Types<std::integral_constant<ReturnMode, ReturnMode::kReturnResultOnError>,
                                     std::integral_constant<ReturnMode, ReturnMode::kAbortOnError>>;

template <typename ModeConstant>
class AdditionFloatingPointsTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(AdditionFloatingPointsTest, ReturnModes, /* unused */);

TYPED_TEST(AdditionFloatingPointsTest, CanAddTwoFloats)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{2.0};
    const double rhs{3.1};
    const auto result = Add<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, lhs + rhs);
}

TYPED_TEST(AdditionFloatingPointsTest, CanAddFloatWithInteger)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{3.1};
    const std::uint32_t rhs{2U};
    const auto result = Add<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, 5.1);
}

TYPED_TEST(AdditionFloatingPointsTest, AdditionFailsIfIntegerCanNotBeRepresentedInFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{3.1};
    const auto rhs{std::numeric_limits<std::uint64_t>::max()};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Add<kMode>(lhs, rhs);
        },
        ErrorCode::kImplicitRounding);
}

TYPED_TEST(AdditionFloatingPointsTest, CanAddIntegerWithFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const std::uint32_t lhs{2U};
    const double rhs{3.1};
    const auto result = Add<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, 5.1);
}

template <typename TypeWithModeParam>
class SubtractTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(SubtractTest, WithBothModes<UnsignedTypes>::type, /* unused */);

TYPED_TEST(SubtractTest, SubtractBiggerUnsignedFromSmallerUnsignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{1U};
    const Type val2{2U};
    Helper::ExpectSuccess(Subtract<kMode, std::int8_t>(val1, val2), std::int8_t{-1});
}

TYPED_TEST(SubtractTest, SubtractEqualUnsignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{2U};
    const Type val2{2U};
    Helper::ExpectSuccess(Subtract<kMode>(val1, val2), Type{0});
}

TYPED_TEST(SubtractTest, SubtractSmallerUnsignedFromBiggerUnsignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{2U};
    const Type val2{1U};
    Helper::ExpectSuccess(Subtract<kMode>(val1, val2), Type{1});
}

TYPED_TEST(SubtractTest, SubtractNegativeSignedFromUnsignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{2U};
    const typename std::make_signed_t<Type> val2{-1};
    Helper::ExpectSuccess(Subtract<kMode>(val1, val2), Type{3});
}

TYPED_TEST(SubtractTest, SubtractPositiveSignedFromUnsignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const Type val1{2U};
    const typename std::make_signed_t<Type> val2{1};
    Helper::ExpectSuccess(Subtract<kMode>(val1, val2), Type{1});
}

TYPED_TEST(SubtractTest, SubtractUnsignedFromPositiveSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{0};
    const Type val2{static_cast<Type>(std::numeric_limits<typename std::make_signed_t<Type>>::max()) + 1U};
    Helper::ExpectSuccess(Subtract<kMode>(val1, val2), std::numeric_limits<typename std::make_signed_t<Type>>::min());
}

TYPED_TEST(SubtractTest, SubtractUnsignedFromNegativeSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto lowest{std::numeric_limits<typename std::make_signed_t<Type>>::lowest()};
    const Type val2{2U};
    const auto val1{lowest + static_cast<typename std::make_signed_t<Type>>(val2)};
    using ExpectedType = ExtractValueType_t<decltype(Subtract<kMode>(val1, val2))>;
    Helper::ExpectSuccess(Subtract<kMode>(val1, val2), static_cast<ExpectedType>(lowest));
}

TEST(SubtractTestAddition, SubtractUnsignedFromNegativeSignedReturnsErrorOnOverflow)
{
    const std::int32_t val1{-1};
    const auto val2{std::numeric_limits<std::uint64_t>::max()};
    const auto result = Subtract(val1, val2);
    ASSERT_FALSE(result.has_value()) << "Value: " << result.value();
    EXPECT_EQ(result.error(), ErrorCode::kExceedsNumericLimits);
}

TYPED_TEST(SubtractTest, SubtractNegativeSignedFromSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename std::make_signed_t<Type> val1{0};
    const typename std::make_signed_t<Type> val2{-2};
    using ExpectedType = ExtractValueType_t<decltype(Subtract<kMode>(val1, val2))>;
    Helper::ExpectSuccess(Subtract<kMode>(val1, val2), static_cast<ExpectedType>(Abs(val2)));
}

TYPED_TEST(SubtractTest, SubtractLowestNegativeSignedFromSignedWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto lowest{std::numeric_limits<typename std::make_signed_t<Type>>::lowest()};
    const typename std::make_signed_t<Type> val1{0};
    const auto val2{lowest};
    Helper::ExpectSuccess(Subtract<kMode, Type>(val1, val2), Abs(val2));
}

TYPED_TEST(SubtractTest, SubtractPositiveSignedMaximumFromSignedZeroWorks)
{
    using Type = typename TypeParam::Type;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto max{std::numeric_limits<typename std::make_signed_t<Type>>::max()};
    const typename std::make_signed_t<Type> val1{0};
    const auto val2{max};
    auto expected_result = Negate<kMode>(max);
    Helper::ExpectSuccess(Subtract<kMode>(val1, val2), Helper::GetValue(expected_result));
}

template <typename TypePairWithModeParam>
class SubtractionMultipleTypesTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(SubtractionMultipleTypesTest);

TYPED_TEST_P(SubtractionMultipleTypesTest, SubtractionWorks)
{
    using TypePair = typename TypeParam::TypePair;
    constexpr auto kMode = TypeParam::kMode;
    using Helper = ReturnModeTestHelper<kMode>;

    const typename TypePair::first_type val1{3};
    const typename TypePair::second_type val2{2};
    Helper::ExpectSuccess(Subtract<kMode>(val1, val2), typename TypePair::first_type{1});
}

REGISTER_TYPED_TEST_SUITE_P(SubtractionMultipleTypesTest, SubtractionWorks);

INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedTypesReturnResult,
                               SubtractionMultipleTypesTest,
                               WithBothModesPairs<UnsignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedTypesReturnResult,
                               SubtractionMultipleTypesTest,
                               WithBothModesPairs<SignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(SignedToUnsignedTypesReturnResult,
                               SubtractionMultipleTypesTest,
                               WithBothModesPairs<SignedUnsignedTypePairs>::type,
                               /* unused */);
INSTANTIATE_TYPED_TEST_SUITE_P(UnsignedToSignedTypesReturnResult,
                               SubtractionMultipleTypesTest,
                               WithBothModesPairs<UnsignedSignedTypePairs>::type,
                               /* unused */);

template <typename ModeConstant>
class SubtractionFloatingPointsTest : public ::testing::Test
{
};

TYPED_TEST_SUITE(SubtractionFloatingPointsTest, ReturnModes, /* unused */);

TYPED_TEST(SubtractionFloatingPointsTest, CanSubtractTwoFloats)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{3.1};
    const double rhs{2.2};
    const auto result = Subtract<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, lhs - rhs);
}

TYPED_TEST(SubtractionFloatingPointsTest, CanSubtractIntegerFromFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{3.1};
    const std::uint32_t rhs{2U};
    const auto result = Subtract<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, 1.1);
}

TYPED_TEST(SubtractionFloatingPointsTest, SubtractionFailsIfRhsIntegerCanNotBeRepresentedInFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const double lhs{3.1};
    const auto rhs{std::numeric_limits<std::uint64_t>::max()};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Subtract<kMode>(lhs, rhs);
        },
        ErrorCode::kImplicitRounding);
}

TYPED_TEST(SubtractionFloatingPointsTest, CanSubtractFloatFromInteger)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const std::uint32_t lhs{2U};
    const double rhs{3.1};
    const auto result = Subtract<kMode>(lhs, rhs);
    Helper::ExpectSuccess(result, -1.1);
}

TYPED_TEST(SubtractionFloatingPointsTest, SubtractionFailsIfLhsIntegerCanNotBeRepresentedInFloat)
{
    constexpr auto kMode = TypeParam::value;
    using Helper = ReturnModeTestHelper<kMode>;

    const auto lhs{std::numeric_limits<std::uint64_t>::max()};
    const double rhs{3.1};
    Helper::ExpectErrorFromOperation(
        [&]() {
            return Subtract<kMode>(lhs, rhs);
        },
        ErrorCode::kImplicitRounding);
}

TEST(AddTypefirstConvenienceOverloadTest, AddWithExplicitReturnTypeUsesDefaultReturnMode)
{
    using Helper = ReturnModeTestHelper<ReturnMode::kReturnResultOnError>;

    const std::int32_t val1{2};
    const std::int32_t val2{3};
    const auto result = Add<std::int64_t>(val1, val2);
    Helper::ExpectSuccess(result, std::int64_t{5});
}

TEST(AddTypefirstConvenienceOverloadTest, AddOverflowWithExplicitReturnTypeReturnsError)
{
    using Helper = ReturnModeTestHelper<ReturnMode::kReturnResultOnError>;

    const std::int32_t val1{std::numeric_limits<std::int32_t>::max()};
    const std::int32_t val2{1};
    const auto result = Add<std::int32_t>(val1, val2);
    Helper::ExpectError(result, ErrorCode::kExceedsNumericLimits);
}

TEST(SubtractTypefirstConvenienceOverloadTest, SubtractWithExplicitReturnTypeUsesDefaultReturnMode)
{
    using Helper = ReturnModeTestHelper<ReturnMode::kReturnResultOnError>;

    const std::int32_t val1{5};
    const std::int32_t val2{3};
    const auto result = Subtract<std::int64_t>(val1, val2);
    Helper::ExpectSuccess(result, std::int64_t{2});
}

TEST(SubtractTypefirstConvenienceOverloadTest, SubtractUnderflowWithExplicitReturnTypeReturnsError)
{
    using Helper = ReturnModeTestHelper<ReturnMode::kReturnResultOnError>;

    const std::uint32_t val1{0U};
    const std::uint32_t val2{1U};
    const auto result = Subtract<std::uint32_t>(val1, val2);
    Helper::ExpectError(result, ErrorCode::kExceedsNumericLimits);
}

}  // namespace
}  // namespace score::safe_math
