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
#include "score/filesystem/iterator/directory_entry.h"

#include "score/filesystem/i_standard_filesystem.h"

namespace score
{
namespace filesystem
{

DirectoryEntry::DirectoryEntry() noexcept = default;
DirectoryEntry::DirectoryEntry(const DirectoryEntry&) noexcept = default;
DirectoryEntry::DirectoryEntry(DirectoryEntry&&) noexcept = default;
// Different LCOV behavior: covered on Linux but not on QNX8, excluded from coverage for QNX8
DirectoryEntry& DirectoryEntry::operator=(const DirectoryEntry&) noexcept = default;  // LCOV_EXCL_LINE
DirectoryEntry& DirectoryEntry::operator=(DirectoryEntry&&) noexcept = default;
DirectoryEntry::~DirectoryEntry() noexcept = default;

DirectoryEntry::DirectoryEntry(const Path& path) noexcept : current_entry_{path} {}

const Path& DirectoryEntry::GetPath() const noexcept
{
    return current_entry_;
}

score::Result<bool> DirectoryEntry::Exists() const noexcept
{
    return filesystem::IStandardFilesystem::instance().Exists(GetPath());
}

score::Result<FileStatus> DirectoryEntry::Status() const noexcept
{
    return filesystem::IStandardFilesystem::instance().Status(GetPath());
}

score::Result<FileStatus> DirectoryEntry::SymlinkStatus() const noexcept
{
    return filesystem::IStandardFilesystem::instance().SymlinkStatus(GetPath());
}

bool operator==(const DirectoryEntry& l, const DirectoryEntry& r) noexcept
{
    return l.current_entry_ == r.current_entry_;
}

bool operator!=(const DirectoryEntry& l, const DirectoryEntry& r) noexcept
{
    return !(l == r);
}

}  // namespace filesystem
}  // namespace score
