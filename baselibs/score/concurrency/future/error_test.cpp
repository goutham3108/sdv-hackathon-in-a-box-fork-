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
#include "score/concurrency/future/error.h"

#include "gtest/gtest.h"

namespace score
{
namespace concurrency
{
namespace
{

class ErrorTest : public ::testing::Test
{
  protected:
    void TestErrorMessage(Error errorCode, std::string_view expectedErrorOutput) const noexcept
    {
        const auto errorCodeTest =
            future_error_domain_dummy_.MessageFor(static_cast<score::result::ErrorCode>(errorCode));
        ASSERT_EQ(errorCodeTest, expectedErrorOutput);
    }

    FutureErrorDomain future_error_domain_dummy_{};
};

TEST_F(ErrorTest, MessageForPromiseBroken)
{
    TestErrorMessage(Error::kPromiseBroken, "Promise broken");
}

TEST_F(ErrorTest, MessageForFutureAlreadyRetrieved)
{
    TestErrorMessage(Error::kFutureAlreadyRetrieved, "Future already retrieved");
}

TEST_F(ErrorTest, MessageForPromiseAlreadySatisfied)
{
    TestErrorMessage(Error::kPromiseAlreadySatisfied, "Promise already satisfied");
}

TEST_F(ErrorTest, MessageForNoState)
{
    TestErrorMessage(Error::kNoState, "No shared state associated");
}

TEST_F(ErrorTest, MessageForStopRequested)
{
    TestErrorMessage(Error::kStopRequested, "Stop requested");
}

TEST_F(ErrorTest, MessageForTimeout)
{
    TestErrorMessage(Error::kTimeout, "Timeout");
}

TEST_F(ErrorTest, MessageForUnset)
{
    TestErrorMessage(Error::kUnset, "Value was not set");
}

TEST_F(ErrorTest, MessageForUnknown)
{
    TestErrorMessage(Error::kUnknown, "Unknown error");
}

}  // namespace
}  // namespace concurrency
}  // namespace score
