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
#include "score/filesystem/file_status.h"

namespace score
{
namespace filesystem
{

FileStatus::FileStatus() noexcept : FileStatus{FileType::kNone} {}
FileStatus::FileStatus(const FileType type, const Perms permissions) noexcept : type_{type}, permissions_{permissions}
{
}

FileStatus::FileStatus(const FileStatus&) noexcept = default;
FileStatus::FileStatus(FileStatus&&) noexcept = default;

// Different LCOV behavior: covered on Linux but not on QNX8, excluded from coverage for QNX8
FileStatus& FileStatus::operator=(const FileStatus&) noexcept = default;  // LCOV_EXCL_LINE
FileStatus& FileStatus::operator=(FileStatus&&) noexcept = default;

FileStatus::~FileStatus() noexcept = default;

FileType FileStatus::Type() const noexcept
{
    return type_;
}

void FileStatus::Type(const FileType type) noexcept
{
    type_ = type;
}

Perms FileStatus::Permissions() const noexcept
{
    return permissions_;
}

void FileStatus::Permissions(const Perms permissions) noexcept
{
    permissions_ = permissions;
}

bool operator==(const FileStatus& lhs, const FileStatus& rhs) noexcept
{
    return (lhs.Type() == rhs.Type()) && (lhs.Permissions() == rhs.Permissions());
}

}  // namespace filesystem
}  // namespace score
