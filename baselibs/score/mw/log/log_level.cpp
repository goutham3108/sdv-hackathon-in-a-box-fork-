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

namespace score
{
namespace mw
{
namespace log
{

mw::log::LogLevel GetLogLevelFromU8(std::uint8_t candidate_log_level)
{
    if (candidate_log_level <= static_cast<std::uint8_t>(GetMaxLogLevelValue()))
    {
        return static_cast<LogLevel>(candidate_log_level);
    }
    else
    {
        return LogLevel::kOff;
    }
}

std::optional<mw::log::LogLevel> TryGetLogLevelFromU8(std::uint8_t candidate_log_level)
{
    if (candidate_log_level <= static_cast<std::uint8_t>(GetMaxLogLevelValue()))
    {
        return static_cast<LogLevel>(candidate_log_level);
    }
    else
    {
        return std::nullopt;
    }
}

}  // namespace log
}  // namespace mw
}  // namespace score
