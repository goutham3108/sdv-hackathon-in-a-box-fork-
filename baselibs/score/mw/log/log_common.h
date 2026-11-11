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

#ifndef SCORE_MW_LOG_COMMON_H
#define SCORE_MW_LOG_COMMON_H

#include <string_view>
#include <typeindex>
#include <unordered_map>

namespace score
{
namespace mw
{
namespace log
{

namespace detail
{
using PeriodSuffixMap = std::unordered_map<std::type_index, std::string_view>;

extern const PeriodSuffixMap kPeriodToSuffix;
}  // namespace detail

template <typename Period>
std::string_view DurationUnitSuffix() noexcept
{
    auto result = detail::kPeriodToSuffix.find(typeid(typename Period::type));
    if (result != detail::kPeriodToSuffix.end())  // LCOV_EXCL_BR_LINE : else branch cannot be reached
    {
        return result->second;
    }
    else
    {
        // should never reach here
        return std::string_view{"not support unit"};  // LCOV_EXCL_LINE : unreachable
    }
}

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_COMMON_H
