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
#include "score/mw/log/log_level.h"

#include "gtest/gtest.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{

TEST(LogLevelTesting, EnsureMaxLevelCoversAllEnumCases)
{
    const score::mw::log::LogLevel max_log_level = GetMaxLogLevelValue();
    //  Test conditions are intentionally put into switch to enforce covering all enum values:
    switch (max_log_level)
    {
        case LogLevel::kVerbose:
            EXPECT_EQ(LogLevel::kVerbose, max_log_level);
            break;
        case LogLevel::kDebug:
        case LogLevel::kInfo:
        case LogLevel::kWarn:
        case LogLevel::kError:
        case LogLevel::kFatal:
        case LogLevel::kOff:
        default:
            FAIL();
            break;
    }
}

TEST(LogLevelTesting, EnsureThatGetLogLevelFromU8WillReturnTheCandidateLogLevelIfItIsWithinTheLogLevelEnumValues)
{
    // Let's pick any value within the LogLevel Enum class.
    constexpr std::uint8_t kLogLevelU8{3};
    const auto log_level = GetLogLevelFromU8(kLogLevelU8);

    EXPECT_EQ(log_level, LogLevel::kWarn);
}

TEST(LogLevelTesting, EnsureThatGetLogLevelFromU8WillReturnKOffIfTheCandidateLogLevelIsNotWithinTheLogLevelEnumValues)
{
    // Let's take any value not within the LogLevel Enum class.
    constexpr std::uint8_t kLogLevelU8{8};
    const auto log_level = GetLogLevelFromU8(kLogLevelU8);

    EXPECT_EQ(log_level, LogLevel::kOff);
}

TEST(LogLevelTesting, EnsureThatTryGetLogLevelFromU8WillReturnTheCandidateLogLevelIfItIsWithinTheLogLevelEnumValues)
{
    // Let's pick any value within the LogLevel Enum class.
    constexpr std::uint8_t kLogLevelU8{3};
    const auto log_level = TryGetLogLevelFromU8(kLogLevelU8);

    EXPECT_EQ(log_level.value(), LogLevel::kWarn);
}

TEST(LogLevelTesting,
     EnsureThatTryGetLogLevelFromU8WillReturnNullIfTheCandidateLogLevelIsNotWithinTheLogLevelEnumValues)
{
    // Let's take any value not within the LogLevel Enum class.
    constexpr std::uint8_t kLogLevelU8{8};
    const auto log_level = TryGetLogLevelFromU8(kLogLevelU8);

    EXPECT_FALSE(log_level.has_value());
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
