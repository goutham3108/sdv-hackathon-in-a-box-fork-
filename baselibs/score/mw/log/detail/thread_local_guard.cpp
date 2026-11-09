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
#include "score/mw/log/detail/thread_local_guard.h"

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

bool& GetThreadLocalFlagFile() noexcept
{
    thread_local bool is_in_logging_stack{false};

    // Returning address of thread_local variable is ok here because it can
    // only be accessed from the same thread
    // coverity[autosar_cpp14_m7_5_2_violation]
    return is_in_logging_stack;
}

}  // namespace

ThreadLocalGuard::ThreadLocalGuard()
{
    GetThreadLocalFlagFile() = true;
}

ThreadLocalGuard::~ThreadLocalGuard()
{
    GetThreadLocalFlagFile() = false;
}

bool ThreadLocalGuard::IsWithingLogging() noexcept
{
    return GetThreadLocalFlagFile();
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
