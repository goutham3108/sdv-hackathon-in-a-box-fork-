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
#ifndef THREAD_LOAD_TRACKING_TOKEN_STATE_H
#define THREAD_LOAD_TRACKING_TOKEN_STATE_H

#include <chrono>
#include <cstdint>

namespace score
{
namespace concurrency
{

/// \brief The state that is assigned when creating a token.
enum class ThreadLoadTrackingState : std::uint8_t
{
    ///\brief Indicates that the thread is currently working.
    kWorking,

    /// \brief Indicates that the thread is currently in a waiting mode.
    kWaiting
};

}  // namespace concurrency
}  // namespace score
#endif
