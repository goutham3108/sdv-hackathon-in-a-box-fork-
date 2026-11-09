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
#ifndef SCORE_LANGUAGE_SAFECPP_SAFE_MATH_DETAILS_RETURN_MODE_TEST_HELPER_H
#define SCORE_LANGUAGE_SAFECPP_SAFE_MATH_DETAILS_RETURN_MODE_TEST_HELPER_H

#include "score/language/safecpp/safe_math/error.h"
#include "score/language/safecpp/safe_math/return_mode.h"

#include "score/result/result.h"

#include <gtest/gtest.h>

#include <type_traits>
#include <utility>

namespace score::safe_math
{

/// \brief Helper to combine a single type with a ReturnMode for typed tests
template <typename T, ReturnMode Mode>
struct TypeWithMode
{
    using Type = T;
    static constexpr ReturnMode kMode = Mode;
};

/// \brief Helper to combine a type pair with a ReturnMode for typed tests
template <typename TPair, ReturnMode Mode>
struct TypePairWithMode
{
    using TypePair = TPair;
    static constexpr ReturnMode kMode = Mode;
};

/// \brief Helper for combining single types with both ReturnModes
template <typename Types>
struct WithBothModes;

template <typename... Ts>
struct WithBothModes<::testing::Types<Ts...>>
{
    using type = ::testing::Types<TypeWithMode<Ts, ReturnMode::kReturnResultOnError>...,
                                  TypeWithMode<Ts, ReturnMode::kAbortOnError>...>;
};

/// \brief Helper for combining type pairs with both ReturnModes
template <typename Types>
struct WithBothModesPairs;

template <typename... Pairs>
struct WithBothModesPairs<::testing::Types<Pairs...>>
{
    using type = ::testing::Types<TypePairWithMode<Pairs, ReturnMode::kReturnResultOnError>...,
                                  TypePairWithMode<Pairs, ReturnMode::kAbortOnError>...>;
};

/// \brief Test helper utility for handling different ReturnMode behaviors in tests.
/// This utility provides specializations for each ReturnMode to avoid test duplication.
template <ReturnMode Mode>
struct ReturnModeTestHelper;

/// \brief Specialization for kReturnResultOnError mode, returns Result<T> wrapper
template <>
struct ReturnModeTestHelper<ReturnMode::kReturnResultOnError>
{
    /// \brief Verifies a successful operation that returns Result<T>
    /// \tparam T The wrapped result type
    /// \param result The Result<T> returned from the operation
    /// \param expected_value The expected value inside Result
    template <typename T>
    static void ExpectSuccess(const Result<T>& result, const T& expected_value)
    {
        ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
        EXPECT_EQ(result.value(), expected_value);
    }

    /// \brief Verifies a successful operation without checking the exact value
    /// \tparam T The wrapped result type
    /// \param result The Result<T> returned from the operation
    template <typename T>
    static void ExpectHasValue(const Result<T>& result)
    {
        ASSERT_TRUE(result.has_value()) << "Error: " << result.error();
    }

    /// \brief Extracts the value from a Result<T>
    /// \tparam T The wrapped result type
    /// \param result The Result<T> returned from the operation
    /// \return The value inside the Result
    /// \pre Result must have a value (call ExpectHasValue first)
    template <typename T>
    static T GetValue(const Result<T>& result)
    {
        return result.value();
    }

    /// \brief Verifies an operation that should return an error
    /// \tparam T The wrapped result type
    /// \param result The Result<T> returned from the operation
    /// \param expected_error The expected error code
    template <typename T>
    static void ExpectError(const Result<T>& result, ErrorCode expected_error)
    {
        ASSERT_FALSE(result.has_value()) << "Value: " << result.value();
        EXPECT_EQ(result.error(), expected_error);
    }

    /// \brief Executes an operation and verifies it returns an error
    /// \tparam Func Callable type that returns Result<T>
    /// \param func The operation to execute
    /// \param expected_error The expected error code
    template <typename Func>
    static void ExpectErrorFromOperation(Func&& func, ErrorCode expected_error)
    {
        const auto result = std::forward<Func>(func)();
        using ResultType = decltype(result);
        using ValueType = typename ResultType::value_type;
        ExpectError<ValueType>(result, expected_error);
    }
};

/// \brief Specialization for kAbortOnError mode, returns direct values and aborts on error
template <>
struct ReturnModeTestHelper<ReturnMode::kAbortOnError>
{
    /// \brief Verifies a successful operation that returns direct value
    /// \tparam T The direct result type
    /// \param result The direct value returned from the operation
    /// \param expected_value The expected value
    template <typename T>
    static void ExpectSuccess(const T& result, const T& expected_value)
    {
        EXPECT_EQ(result, expected_value);
    }

    /// \brief Verifies a successful operation without checking the exact value (no-op for abort mode)
    /// \tparam T The direct result type
    /// \param result The direct value returned from the operation
    /// \note This is a no-op since reaching this point means the operation succeeded
    template <typename T>
    static void ExpectHasValue(const T& /* result */)
    {
        // For AbortOnError mode, if we got here, the operation succeeded
        // No need to check anything as the value exists by definition
    }

    /// \brief Extracts the value (identity function for abort mode since result is already the value)
    /// \tparam T The direct result type
    /// \param result The direct value returned from the operation
    /// \return The value itself
    template <typename T>
    static T GetValue(const T& result)
    {
        return result;
    }

    /// \brief Verifies an operation aborts when executed (death test)
    /// \tparam Func Callable type that would abort
    /// \param func The operation to execute
    /// \param expected_error Unused in abort mode (kept for API compatibility)
    template <typename Func>
    static void ExpectErrorFromOperation(Func&& func, ErrorCode /* expected_error */)
    {
        EXPECT_DEATH(std::forward<Func>(func)(), ".*");
    }
};

}  // namespace score::safe_math

#endif  // SCORE_LANGUAGE_SAFECPP_SAFE_MATH_DETAILS_RETURN_MODE_TEST_HELPER_H
