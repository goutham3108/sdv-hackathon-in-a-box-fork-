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
#include <cstdlib>
#include <exception>

/// @brief prevent creation of exception objects by aborting immediately
/// @details see broken_link_c/issue/6593458
extern "C" void* __cxa_allocate_exception(size_t) noexcept
{
    // Calling std::abort() here is intended since an intermediate terminate handler could do things that we don't want.
    // In safety-related applications we want to terminate as immediate as possible for reaching the safe state.
    // And that is in this case the abortion of the violating application which caused the C++ exception.
    std::abort();
}
