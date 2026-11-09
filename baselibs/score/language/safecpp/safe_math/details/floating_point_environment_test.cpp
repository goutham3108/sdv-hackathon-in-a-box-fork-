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
#include "score/language/safecpp/safe_math/details/floating_point_environment.h"
#include "score/language/safecpp/safe_math/error.h"
#include "score/language/safecpp/safe_math/return_mode.h"

#include <score/callback.hpp>

#include "gtest/gtest.h"

#include <cfenv>

namespace score::safe_math
{
namespace
{

class FloatingPointEnvironmentTest : public ::testing::Test
{
  protected:
    void SetUp() noexcept override
    {
#if defined(VALGRIND)
        GTEST_SKIP() << "Valrgind has some limitations with the FPU and we cannot run the test";
#endif
    }

    const score::cpp::callback<double()> valid_operation_{[]() noexcept {
        return 1.0 + 2.0;
    }};
    const score::cpp::callback<double()> invalid_operation_{[]() noexcept {
        return 1.0 / 0.0;
    }};
};

TEST_F(FloatingPointEnvironmentTest, CanConstructAndDestructWithoutError)
{
    EXPECT_NO_FATAL_FAILURE((details::FloatingPointEnvironment{}));
}

TEST_F(FloatingPointEnvironmentTest, WillCatchError)
{
    details::FloatingPointEnvironment floating_point_environment{};
    static_cast<void>(invalid_operation_());
    const auto error = floating_point_environment.Test();
    ASSERT_FALSE(error.has_value());
    EXPECT_EQ(error.error(), ErrorCode::kUnknown);
}

TEST_F(FloatingPointEnvironmentTest, WillNotLeakError)
{
    {
        details::FloatingPointEnvironment floating_point_environment{};
        static_cast<void>(invalid_operation_());
        const auto error = floating_point_environment.Test();
        EXPECT_FALSE(error.has_value());
    }
    EXPECT_FALSE(std::fetestexcept(FE_ALL_EXCEPT) || (errno == EDOM) || (errno == ERANGE));
}

TEST_F(FloatingPointEnvironmentTest, ErrorsWillNotLeakIntoEnvironment)
{
    static_cast<void>(invalid_operation_());
    EXPECT_TRUE(std::fetestexcept(FE_ALL_EXCEPT) || (errno == EDOM) || (errno == ERANGE));
    {
        details::FloatingPointEnvironment floating_point_environment{};
        const auto error = floating_point_environment.Test();
        EXPECT_TRUE(error.has_value());
    }
    EXPECT_FALSE(std::fetestexcept(FE_ALL_EXCEPT) || (errno == EDOM) || (errno == ERANGE));
}

TEST_F(FloatingPointEnvironmentTest, CalculateAndVerifyWillReturnCorrectResultWhenModeIsReturnResultOnError)
{
    const auto result = valid_operation_();
    const auto verified_result =
        details::FloatingPointEnvironment::CalculateAndVerify<ReturnMode::kReturnResultOnError>(valid_operation_);
    ASSERT_TRUE(verified_result.has_value());
    EXPECT_EQ(verified_result.value(), result);
}

TEST_F(FloatingPointEnvironmentTest, CalculateAndVerifyWillReturnErrorWhenModeIsReturnResultOnError)
{
    const auto verified_result =
        details::FloatingPointEnvironment::CalculateAndVerify<ReturnMode::kReturnResultOnError>(invalid_operation_);
    ASSERT_FALSE(verified_result.has_value());
    EXPECT_EQ(verified_result.error(), ErrorCode::kUnknown);
}

TEST_F(FloatingPointEnvironmentTest, CalculateAndVerifyWillReturnCorrectValueWhenModeIsAbortOnError)
{
    // Given, CalculateAndVerify is configured in AbortOnError mode with a valid operation
    const auto result = valid_operation_();
    // When, A valid floating point operation is executed through CalculateAndVerify
    const auto verified_result =
        details::FloatingPointEnvironment::CalculateAndVerify<ReturnMode::kAbortOnError>(valid_operation_);
    // Then, The function should return the computed value directly without Result wrapper
    EXPECT_EQ(verified_result, result);
}

TEST_F(FloatingPointEnvironmentTest, CalculateAndVerifyWillAbortWhenModeIsAbortOnError)
{
    // Given, CalculateAndVerify is configured in AbortOnError mode with an invalid operation
    // When, An invalid floating point operation (division by zero) is executed through CalculateAndVerify
    // Then, The function should call std::abort() instead of returning an error
    EXPECT_DEATH(details::FloatingPointEnvironment::CalculateAndVerify<ReturnMode::kAbortOnError>(invalid_operation_),
                 ".*");
}

}  // namespace
}  // namespace score::safe_math
