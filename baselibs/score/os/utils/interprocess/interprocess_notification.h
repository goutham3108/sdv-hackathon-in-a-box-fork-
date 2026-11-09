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
#ifndef SCORE_LIB_OS_UTILS_INTERPROCESSNOTIFICATION_H
#define SCORE_LIB_OS_UTILS_INTERPROCESSNOTIFICATION_H

#include "score/concurrency/condition_variable.h"
#include "score/concurrency/notification.h"
#include "score/os/utils/interprocess/interprocess_conditional_variable.h"
#include "score/os/utils/interprocess/interprocess_mutex.h"

#include "score/stop_token.hpp"

namespace score
{
namespace os
{

/**
 * \brief The InterprocessNotification allows to receive
 * notifications synchronized by different processes.
 *
 * The main idea is that a process (or thread) are waiting
 * for a specific notification to happen.
 * Another process (or thread) could then `notify()` all waiting
 * processes (or threads) for this notification.
 *
 * It shall be noted that this is not a one-shot solution.
 * After a notification was received threads will again wait until
 * again `notify()` is invoked.
 *
 * As an conditional variable, this class is neither copyable nor moveable.
 *
 * \detail Overall this is only small abstraction around a conditional
 * variable that protects a boolean (if notified was invoked).
 *
 * This class is safe to be stored in shared memory.
 *
 * Even though this class can be used to synchronize threads, it seems
 * more appropriate to use a custom implementation using C++ Standard Library
 * facilities (for performance reasons).
 */
using InterprocessNotification = score::concurrency::NotificationBasic<
    InterprocessMutex,
    score::concurrency::InterruptibleConditionalVariableBasic<InterprocessMutex, InterprocessConditionalVariable>>;

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_INTERPROCESSNOTIFICATION_H
