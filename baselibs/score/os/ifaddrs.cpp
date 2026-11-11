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
///
/// @file ifaddrs.cpp
/// @copyright Copyright (C) 2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// @brief Wrapper class for the ifaddrs.h module
///

#include "score/os/ifaddrs.h"
#include "score/os/ifaddrs_impl.h"

namespace score
{
namespace os
{

Ifaddrs& Ifaddrs::instance() noexcept
{
    static IfaddrsImpl instance;
    return select_instance(instance);
}

}  // namespace os

}  // namespace score
