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
#include "score/os/mocklib/pthreadwrappermock.h"

#include <functional>

namespace score
{
namespace os
{

namespace
{

std::function<std::int32_t(const pthread_t thread, const char* const name)> pthread_setname_np_callback;
std::function<std::int32_t(const pthread_t thread, char* const name, const std::size_t length)>
    pthread_getname_np_callback;

} /* namespace */

PthreadWrapperMock::PthreadWrapperMock()
{
    pthread_setname_np_callback = [this](const pthread_t thread, const char* const name) {
        return this->pthread_setname_np(thread, name);
    };

    pthread_getname_np_callback = [this](const pthread_t thread, char* const name, const std::size_t length) {
        return this->pthread_getname_np(thread, name, length);
    };
}

std::int32_t pthread_setname_np(const pthread_t thread, const char* const name)
{
    return pthread_setname_np_callback(thread, name);
}

#ifdef ALLOW_QM
std::int32_t pthread_getname_np(const pthread_t thread, char* const name, const std::size_t length)
{
    return pthread_getname_np_callback(thread, name, length);
}
#endif  // ALLOW_QM

} /* namespace os */
} /* namespace score */
