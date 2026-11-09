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
#ifndef SCORE_MW_COM_MOCKING_TEST_TYPE_UTILITIES_H
#define SCORE_MW_COM_MOCKING_TEST_TYPE_UTILITIES_H

#include "score/mw/com/mocking/i_runtime.h"

/// @brief test_type_utilities contains all of the factory functions that are required for creating fakes related to
/// mw::com::runtime which are required for mocking.
///
/// These types and functions should not be accessed directly by applications, but rather they should use
/// mw/com/test_types.h
namespace score::mw::com::runtime
{

void InjectRuntimeMockImpl(IRuntime& runtime_mock);

}  // namespace score::mw::com::runtime

#endif  // SCORE_MW_COM_MOCKING_TEST_TYPE_UTILITIES_H
