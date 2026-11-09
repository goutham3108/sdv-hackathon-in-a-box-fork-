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
#include "score/result/error.h"

#include "score/mw/log/logging.h"
#include "score/mw/log/recorder_mock.h"

#include <gtest/gtest.h>

#include <sstream>

namespace score
{
namespace mw
{
namespace log
{

namespace
{

using ::testing::_;
using ::testing::Return;
const mw::log::SlotHandle kHandle{42};

enum class MyErrorCode : score::result::ErrorCode
{
    kFirstError,
    kSecondError,
};

class MyErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        switch (static_cast<MyErrorCode>(code))
        {
            case MyErrorCode::kFirstError:
                return "First Error!";
            case MyErrorCode::kSecondError:
                return "Second Error!";
            default:
                return "Unknown Error!";
        }
    }
};

constexpr MyErrorDomain kMyErrorDomain;

score::result::Error MakeError(MyErrorCode code, std::string_view user_message = "") noexcept
{
    return {static_cast<score::result::ErrorCode>(code), kMyErrorDomain, user_message};
}

class ErrorViaLogStreamFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        mw::log::SetLogRecorder(&recorder);

        EXPECT_CALL(recorder, StartRecord(_, _)).WillOnce(Return(kHandle));
        EXPECT_CALL(recorder, StopRecord(_)).Times(1);
    }

    mw::log::RecorderMock recorder{};
};

TEST_F(ErrorViaLogStreamFixture, CanStreamViaRValueStream)
{
    // Given an constructed error with user message
    const score::result::Error unit{MyErrorCode::kFirstError, "We had a parsing failure"};

    // Expecting that this error message is logged
    EXPECT_CALL(recorder, LogStringView(_, {"Error "}));
    EXPECT_CALL(recorder, LogStringView(_, {"First Error!"}));
    EXPECT_CALL(recorder, LogStringView(_, {" occurred"}));
    EXPECT_CALL(recorder, LogStringView(_, {" with message "}));
    EXPECT_CALL(recorder, LogStringView(_, {"We had a parsing failure"}));

    // When streaming it into an r-value LogStream
    mw::log::LogError() << unit;
}

TEST_F(ErrorViaLogStreamFixture, StreamIntoMwLogStream2)
{
    // Given an constructed error with user message
    const score::result::Error unit{MyErrorCode::kFirstError, "We had a parsing failure"};

    // Expecting that this error message is logged
    EXPECT_CALL(recorder, LogStringView(_, {"Error "}));
    EXPECT_CALL(recorder, LogStringView(_, {"First Error!"}));
    EXPECT_CALL(recorder, LogStringView(_, {" occurred"}));
    EXPECT_CALL(recorder, LogStringView(_, {" with message "}));
    EXPECT_CALL(recorder, LogStringView(_, {"We had a parsing failure"}));

    // When streaming it into an l-value LogStream
    auto stream = mw::log::LogError();
    stream << unit;
}

}  // namespace
}  // namespace log
}  // namespace mw
}  // namespace score
