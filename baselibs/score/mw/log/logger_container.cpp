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
#include "score/mw/log/logger_container.h"

namespace score
{
namespace mw
{
namespace log
{

namespace
{
constexpr std::size_t kMaxLoggersSize{32U};
}

LoggerContainer::LoggerContainer() : stack_{kMaxLoggersSize}, default_logger_{score::mw::log::GetDefaultContextId()} {}

Logger& LoggerContainer::GetLogger(const std::string_view context) noexcept
{
    auto logger = FindExistingLogger(context);

    if (logger.has_value())
    {
        return logger.value();
    }
    return InsertNewLogger(context);
}

Logger& LoggerContainer::InsertNewLogger(const std::string_view context) noexcept
{
    const auto result = stack_.TryPush(Logger{context});
    if (result.has_value())
    {
        return result.value();
    }
    // Returning address of non-static private class member is justified by
    // design, that the logger object ownership stays within the Logging framework.
    // https://www.autosar.org/fileadmin/standards/R20-11/AP/AUTOSAR_SWS_LogAndTrace.pdf section-8.2.1
    // coverity[autosar_cpp14_a9_3_1_violation]
    return default_logger_;
}

score::cpp::optional<std::reference_wrapper<Logger>> LoggerContainer::FindExistingLogger(
    const std::string_view context) noexcept
{
    return stack_.Find([context](const Logger& logger) noexcept {
        return logger.GetContext() == context;
    });
}

size_t LoggerContainer::GetCapacity() const noexcept
{
    return kMaxLoggersSize;
}

Logger& LoggerContainer::GetDefaultLogger() noexcept
{
    // Returning address of non-static private class member is justified by
    // design, that the logger object ownership stays within the Logging framework.
    // https://www.autosar.org/fileadmin/standards/R20-11/AP/AUTOSAR_SWS_LogAndTrace.pdf section-8.2.1
    // coverity[autosar_cpp14_a9_3_1_violation]
    return default_logger_;
}

}  // namespace log
}  // namespace mw
}  // namespace score
