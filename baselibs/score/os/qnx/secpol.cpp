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
#include "score/os/qnx/secpol.h"
#include "score/os/qnx/secpol_impl.h"

score::os::qnx::Secpol& score::os::qnx::Secpol::instance() noexcept
{
    static score::os::qnx::details::SecpolImpl instance;
    return select_instance(instance);
}
