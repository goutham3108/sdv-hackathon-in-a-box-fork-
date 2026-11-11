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
#ifndef SCORE_MW_LOG_LOG_STREAM_FACTORY_H
#define SCORE_MW_LOG_LOG_STREAM_FACTORY_H

#include "score/mw/log/log_level.h"
#include "score/mw/log/log_stream.h"

#include <string_view>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
class LogStreamFactory
{
  public:
    /// \brief Will create a `LogStream` based on currently set `Recorder` in the runtime.
    ///        This function shall not be used be the end-user. Please refer only to APIs outside the `detail`
    ///        namespace.
    ///
    /// \details This factory is tested within the LogStream unit tests. If no valid recorder is set, this function will
    ///          abort!
    ///
    /// \param log_level The LogLevel the created stream shall use
    /// \param context_id The context id the created stream shall use
    /// \return A newly created LogStream which can be used to LogData
    static LogStream GetStream(const LogLevel, const std::string_view context_id = "DFLT") noexcept;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_LOG_STREAM_FACTORY_H
