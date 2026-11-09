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

#include "score/mw/log/detail/dlt_argument_counter.h"

#include <limits>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

DltArgumentCounter::DltArgumentCounter(std::uint8_t& counter) noexcept : counter_(counter) {}

AddArgumentResult DltArgumentCounter::TryAddArgument(AddArgumentCallback callback) noexcept
{
    constexpr uint8_t kMaxCounter = std::numeric_limits<std::remove_reference<decltype(counter_)>::type>::max();
    if (counter_ == kMaxCounter)
    {
        return AddArgumentResult::kNotAdded;
    }

    const auto result = callback();
    if (result == AddArgumentResult::kAdded)
    {
        counter_++;
    }

    return result;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
