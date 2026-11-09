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
#include "score/concurrency/destruction_guard.h"

score::concurrency::DestructionGuard::DestructionGuard(std::atomic<std::uint32_t>& counter) noexcept : counter_{counter}
{
    ++counter_;
}

score::concurrency::DestructionGuard::~DestructionGuard() noexcept
{
    --counter_;
}
