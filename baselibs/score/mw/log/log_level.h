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
#ifndef SCORE_MW_LOG_LOG_LEVEL_H
#define SCORE_MW_LOG_LOG_LEVEL_H

// Be careful what you include here. Each additional header will be included in logging.h and thus exposed to the user.
// We need to try to keep the includes low to reduce the compile footprint of using this library.
#include <algorithm>
#include <cstdint>
#include <optional>

namespace score
{
namespace mw
{
namespace log
{

/// \brief Represents the severity of a log message
/// \public
///
/// \details The severity of log messages will be used to filter if a message shall be further processed. This can be
/// used by an end-user to filter messages and reduce performance implications due to extensive logging.
enum class LogLevel : std::uint8_t
{
    kOff = 0x00,
    kFatal = 0x01,
    kError = 0x02,
    kWarn = 0x03,
    kInfo = 0x04,
    kDebug = 0x05,
    kVerbose = 0x06,
};

constexpr LogLevel GetMaxLogLevelValue()
{
    return std::max({LogLevel::kVerbose,
                     LogLevel::kInfo,
                     LogLevel::kWarn,
                     LogLevel::kError,
                     LogLevel::kFatal,
                     LogLevel::kDebug,
                     LogLevel::kOff});
}

mw::log::LogLevel GetLogLevelFromU8(std::uint8_t candidate_log_level);
std::optional<mw::log::LogLevel> TryGetLogLevelFromU8(std::uint8_t candidate_log_level);

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_LOG_LEVEL_H
