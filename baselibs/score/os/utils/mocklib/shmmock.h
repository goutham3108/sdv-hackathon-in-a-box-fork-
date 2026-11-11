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
#ifndef SCORE_LIB_OS_UTILS_MOCKLIB_SHMMOCK_H
#define SCORE_LIB_OS_UTILS_MOCKLIB_SHMMOCK_H

#include "score/os/utils/shm.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

struct ShmMock
{
    ShmMock();

    MOCK_METHOD(void, ShmWrapperMock_constructor, (const std::string&, std::size_t));
    MOCK_METHOD(void, ShmWrapperMock_destructor, ());

    MOCK_METHOD(void, unlink, ());
    MOCK_METHOD(uint8_t*, get, ());
};

}  // namespace os
}  // namespace score

#endif
