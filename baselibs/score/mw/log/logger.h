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
#ifndef SCORE_MW_LOG_LOGGER_H
#define SCORE_MW_LOG_LOGGER_H

#include "score/mw/log/log_stream.h"

#include <string_view>

#include "score/mw/log/detail/logging_identifier.h"

namespace score
{
namespace mw
{
namespace log
{

/// \brief The logger creates LogStreams with a user-defined context.
/// Implicitly generated assignment operators are NOT thread-safe.
class Logger final
{
  public:
    /// \brief Constructs a Logger object with a given context. All subsequent calls to a log statement will be logged
    /// under the provided context.
    /// \public
    /// \thread-safe
    explicit Logger(const std::string_view context) noexcept;

    /* Reusing identifiers allowed to keep interface consistent betweeen the member functions in this class with the
     * free functions defined in logging.h. */
    /// \brief Creates a LogStream to log messages of criticality `Fatal` (highest).
    /// \public
    /// \thread-safe
    ///
    /// \details Fatal shall be used on errors that cannot be recovered and will lead to an overall failure in the
    /// system. The message will be logged under the context that was provided on construction. \return LogStream which
    /// can be used stream verbose logging messages (will be flushed on destruction)
    log::LogStream LogFatal() const noexcept;

    /// \brief Creates a LogStream to log messages of criticality `Error` (2nd highest).
    /// \public
    /// \thread-safe
    ///
    /// \details Error shall be used on errors that can be recovered and would lead to an failure in the system.
    /// The message will be logged under the context that was provided on construction.
    /// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
    log::LogStream LogError() const noexcept;

    /// \brief Creates a LogStream to log messages of criticality `Warn` (3rd highest).
    /// \public
    /// \thread-safe
    ///
    /// \details Warnings shall be used on errors that might be no error but desired state or could to an error later
    /// on. The message will be logged under the context that was provided on construction. \return LogStream which can
    /// be used stream verbose logging messages (will be flushed on destruction)
    log::LogStream LogWarn() const noexcept;

    /// \brief Creates a LogStream to log messages of criticality `Info` (4th highest).
    /// \public
    /// \thread-safe
    ///
    /// \details Infos shall be used on messages that are of interest to analyze issues and understand overall program
    /// flow. The message will be logged under the context that was provided on construction. \return LogStream which
    /// can be used stream verbose logging messages (will be flushed on destruction)
    log::LogStream LogInfo() const noexcept;

    /// \brief Creates a LogStream to log messages of criticality `Debug` (5th highest).
    /// \public
    /// \thread-safe
    ///
    /// \details Debug shall be used on messages that are of interest to analyze issues in depth.
    /// The message will be logged under the context that was provided on construction.
    /// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
    log::LogStream LogDebug() const noexcept;

    /// \brief Creates a LogStream to log messages of criticality `Verbose` (lowest).
    /// \public
    /// \thread-safe
    ///
    /// \details Verbose shall be used on messages that are of interest to analyze issues in depth but lead to super
    /// high bandwidth (e.g. sending every millisecond). The message will be logged under the context that was provided
    /// on construction.
    /// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
    log::LogStream LogVerbose() const noexcept;

    /// \brief Log a message where the log level is determined by an agurment.
    /// \public
    /// \thread-safe
    /// \details See also AUTOSAR_SWS_LogAndTrace R20-11, Section 8.3.2.8
    log::LogStream WithLevel(const LogLevel log_level) const noexcept;

    /// \brief Check if the log level is enabled for the current context.
    /// \public
    /// \thread-safe
    /// \details See also AUTOSAR_SWS_LogAndTrace R20-11, Section 8.3.2.7
    bool IsLogEnabled(const LogLevel) const noexcept;

    /// \brief Check if the log level is enabled for the current context.
    /// \public
    /// \thread-safe
    /// \details See also AUTOSAR_SWS_LogAndTrace R20-11, Section 8.3.2.7
    bool IsEnabled(const LogLevel) const noexcept;

    std::string_view GetContext() const noexcept;

  private:
    detail::LoggingIdentifier context_;
};

score::mw::log::Logger& CreateLogger(const std::string_view context) noexcept;
score::mw::log::Logger& CreateLogger(const std::string_view context_id, const std::string_view) noexcept;
const std::string& GetDefaultContextId() noexcept;

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_LOGGER_H
