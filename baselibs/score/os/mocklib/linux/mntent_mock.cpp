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
#include <functional>

#include "mntent_mock.h"

namespace score
{
namespace os
{

namespace
{
std::function<FILE*(const char* file, const char* mode)> setMntent_callback;
std::function<struct mntent*(FILE* stream)> getMntent_callback;
std::function<int(FILE* stream)> endMntent_callback;
} /* namespace */

MntentMock::MntentMock()
{
    setMntent_callback = [this](const char* file, const char* mode) {
        return this->setMntent(file, mode);
    };
    getMntent_callback = [this](FILE* stream) {
        return this->getMntent(stream);
    };
    endMntent_callback = [this](FILE* stream) {
        return this->endMntent(stream);
    };
}

FILE* setMntent(const char* file, const char* mode)
{
    return setMntent_callback(file, mode);
}

struct mntent* getMntent(FILE* stream)
{
    return getMntent_callback(stream);
}

int endMntent(FILE* stream)
{
    return endMntent_callback(stream);
}

} /* namespace os */
} /* namespace score */
