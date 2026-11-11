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
#include "score/os/mocklib/errno_mock.h"

#include <functional>

namespace score
{
namespace os
{

namespace
{

std::function<std::int32_t()> geterrno_callback;
std::function<void(std::int32_t)> seterrno_callback;

} /* namespace */

ErrorMock::ErrorMock()
{

    geterrno_callback = [this]() {
        return this->geterrno();
    };
    seterrno_callback = [this](std::int32_t new_errno) {
        return this->seterrno(new_errno);
    };
}

std::int32_t geterrno()
{
    return geterrno_callback();
}

void seterrno(std::int32_t new_errno)
{
    return seterrno_callback(new_errno);
}

} /* namespace os */
} /* namespace score */
