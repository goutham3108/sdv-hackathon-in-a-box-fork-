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
#include "score/mw/log/logger.h"
#include "score/mw/log/log_stream_factory.h"
#include "score/mw/log/logging.h"
#include "score/mw/log/runtime.h"

namespace
{
/*
Deviation from Rule A3-3-2:
- Static and thread-local objects shall be constant-initialized
Justification:
- cannot change to std::string_view due to dependency
*/
// coverity[autosar_cpp14_a3_3_2_violation]
const std::string kDefaultContext{"DFLT"};
}  // namespace

namespace score
{
namespace mw
{
namespace log
{

Logger::Logger(const std::string_view context) noexcept
    : context_(context.data() == nullptr ? GetDefaultContextId() : context)
{
}

log::LogStream Logger::LogFatal() const noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kFatal, context_.GetStringView());
}

log::LogStream Logger::LogError() const noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kError, context_.GetStringView());
}

log::LogStream Logger::LogWarn() const noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kWarn, context_.GetStringView());
}

log::LogStream Logger::LogInfo() const noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kInfo, context_.GetStringView());
}

log::LogStream Logger::LogDebug() const noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kDebug, context_.GetStringView());
}

log::LogStream Logger::LogVerbose() const noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kVerbose, context_.GetStringView());
}

log::LogStream Logger::WithLevel(const LogLevel log_level) const noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(log_level, context_.GetStringView());
}

bool Logger::IsLogEnabled(const LogLevel log_level) const noexcept
{
    return IsEnabled(log_level);
}

bool Logger::IsEnabled(const LogLevel log_level) const noexcept
{
    return score::mw::log::detail::Runtime::GetRecorder().IsLogEnabled(log_level, context_.GetStringView());
}

std::string_view Logger::GetContext() const noexcept
{
    return context_.GetStringView();
}

score::mw::log::Logger& CreateLogger(const std::string_view context) noexcept
{
    return score::mw::log::detail::Runtime::GetLoggerContainer().GetLogger(context);
}

score::mw::log::Logger& CreateLogger(const std::string_view context_id, const std::string_view) noexcept
{
    return CreateLogger(context_id);
}

const std::string& GetDefaultContextId() noexcept
{
    return kDefaultContext;
}

}  // namespace log
}  // namespace mw
}  // namespace score
