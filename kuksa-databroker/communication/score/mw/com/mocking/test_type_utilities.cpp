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
#include "score/mw/com/mocking/test_type_utilities.h"

#include "score/mw/com/runtime.h"

namespace score::mw::com::runtime
{
namespace detail
{

/// @brief Function which is a friend of RuntimeMockHolder in mw/com/runtime.h. It allows injecting a mock into the
/// RuntimeMockHolder.
void InjectRuntimeMock(IRuntime& runtime_mock)
{
    RuntimeMockHolder::InjectRuntimeMockImpl(runtime_mock);
}

}  // namespace detail

void InjectRuntimeMockImpl(IRuntime& runtime_mock)
{
    detail::InjectRuntimeMock(runtime_mock);
}

}  // namespace score::mw::com::runtime
