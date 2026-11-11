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
#include "shared_memory_chunk.h"
namespace score
{
namespace analysis
{
namespace tracing
{
// Define the equality operator
// Suppress "AUTOSAR C++14 A15-5-3" rule finding. This rule states:
// "The std::terminate() function shall not be called implicitly".
// Rationale: Calling std::terminate() if any exceptions are thrown
// is expected as per safety requirements
// coverity[autosar_cpp14_a15_5_3_violation]
bool operator==(const SharedMemoryChunk& lhs, const SharedMemoryChunk& rhs) noexcept
{
    auto lhs_data = lhs.GetData();
    auto rhs_data = rhs.GetData();
    if (!lhs_data.has_value() || !rhs_data.has_value())
    {
        return false;  // Corrupted data is not equal to anything
    }
    const auto& lhs_ref = lhs_data.value().get();
    const auto& rhs_ref = rhs_data.value().get();
    return (lhs_ref.size_ == rhs_ref.size_) && (lhs_ref.start_ == rhs_ref.start_);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
