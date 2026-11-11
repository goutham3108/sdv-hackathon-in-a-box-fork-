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
#include "shared_memory_location.h"

namespace score
{
namespace analysis
{
namespace tracing
{

bool operator==(const SharedMemoryLocation& lhs, const SharedMemoryLocation& rhs) noexcept
{
    return ((lhs.offset_ == rhs.offset_) && (lhs.shm_object_handle_ == rhs.shm_object_handle_));
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
