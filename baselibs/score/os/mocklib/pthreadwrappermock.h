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
#ifndef SCORE_LIB_OS_MOCKLIB_PTHREADWRAPPERMOCK_H
#define SCORE_LIB_OS_MOCKLIB_PTHREADWRAPPERMOCK_H

#include "score/os/pthread.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

struct PthreadWrapperMock
{
    PthreadWrapperMock();
    virtual ~PthreadWrapperMock() = default;

    MOCK_METHOD(std::int32_t, pthread_setname_np, (const pthread_t thread, const char* const name));
    MOCK_METHOD(std::int32_t, pthread_getname_np, (const pthread_t thread, char* const name, const std::size_t length));
};

} /* namespace os */
} /* namespace score */

#endif  // SCORE_LIB_OS_MOCKLIB_PTHREADWRAPPERMOCK_H
