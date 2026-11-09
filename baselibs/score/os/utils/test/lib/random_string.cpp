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
#include "score/os/utils/test/lib/random_string.h"

#include <algorithm>
#include <random>
#include <utility>

std::string random_string(std::size_t length)
{
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);

    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0, max_index);

    auto randchar = [&charset, &distr, &eng]() {
        return charset[distr(eng)];
    };

    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return "/" + std::to_string(std::hash<std::string>()(str));
}
