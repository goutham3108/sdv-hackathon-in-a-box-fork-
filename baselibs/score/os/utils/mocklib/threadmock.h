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
#ifndef SCORE_LIB_OS_UTILS_MOCKLIB_THREADMOCK_H
#define SCORE_LIB_OS_UTILS_MOCKLIB_THREADMOCK_H

#include <string>
#include <thread>

#include <gmock/gmock.h>

namespace score
{
namespace os
{

struct ThreadMock
{
    ThreadMock();

    MOCK_METHOD(bool, set_thread_affinity, (const std::size_t), ());
    MOCK_METHOD(void, set_thread_name, (std::thread&, const std::string&), ());
    MOCK_METHOD(std::string, get_thread_name, (std::thread&), ());
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_MOCKLIB_THREADMOCK_H
