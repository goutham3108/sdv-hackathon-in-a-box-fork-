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
#include "score/filesystem/i_standard_filesystem.h"

#include "score/filesystem/details/standard_filesystem.h"

namespace score
{
namespace filesystem
{

IStandardFilesystem::~IStandardFilesystem() noexcept = default;

IStandardFilesystem& IStandardFilesystem::instance() noexcept
{
    // coverity[autosar_cpp14_a3_3_2_violation] select instance accepts &, hence const qual can't be added
    static StandardFilesystem instance{};  // LCOV_EXCL_BR_LINE caused by exception?
    return select_instance(instance);
}

bool IsValid(const PermOptions perm_options) noexcept
{
    constexpr auto k_replace = static_cast<std::uint32_t>(PermOptions::kReplace);
    constexpr auto k_add = static_cast<std::uint32_t>(PermOptions::kAdd);
    constexpr auto k_remove = static_cast<std::uint32_t>(PermOptions::kRemove);
    constexpr auto k_no_follow = static_cast<std::uint32_t>(PermOptions::kNoFollow);
    constexpr std::uint32_t operation_mask = k_replace | k_add | k_remove;
    constexpr std::uint32_t full_mask = operation_mask | k_no_follow;
    const auto options = static_cast<std::uint32_t>(perm_options);
    const std::uint32_t operation = options & operation_mask;
    return (((options & (~full_mask)) == 0U) &&
            ((operation == k_replace) || ((operation == k_add) || (operation == k_remove))));
}

}  // namespace filesystem
}  // namespace score
