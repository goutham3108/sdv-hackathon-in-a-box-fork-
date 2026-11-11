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
#ifndef SCORE_LIB_FILESYSTEM_DIRECTORY_ENTRY_H
#define SCORE_LIB_FILESYSTEM_DIRECTORY_ENTRY_H

#include "score/filesystem/error.h"
#include "score/filesystem/file_status.h"
#include "score/filesystem/path.h"

namespace score
{
namespace filesystem
{

/// @brief Represents a directory entry. The object stores a path as a member and may also store additional file
/// attributes during directory iteration.
///
/// @note That only a subset of functionality has been implemented until now. If you need missing features, feel free to
/// add them.
///
/// @see https://en.cppreference.com/w/cpp/filesystem/directory_entry
class DirectoryEntry
{
  public:
    DirectoryEntry() noexcept;
    DirectoryEntry(const DirectoryEntry&) noexcept;
    DirectoryEntry(DirectoryEntry&&) noexcept;
    DirectoryEntry& operator=(const DirectoryEntry&) noexcept;
    DirectoryEntry& operator=(DirectoryEntry&&) noexcept;
    ~DirectoryEntry() noexcept;

    /// @brief  Initializes the directory entry with path. If an error occurs, the DirectoryEntry will hold a
    /// default-constructed path.
    explicit DirectoryEntry(const Path&) noexcept;

    /// @brief Returns the full path the directory entry refers to.
    ///
    /// @note In difference to the C++ Standard we renamed this function from `Path` to `GetPath` to avoid shadowing -
    /// which is checked via enforced compiler warnings.
    const Path& GetPath() const noexcept;

    /// @brief Checks whether the pointed-to object exists.
    score::Result<bool> Exists() const noexcept;

    /// @brief Returns status of the entry, as if determined by a filesystem::status call (symlinks are followed to
    /// their targets).
    score::Result<FileStatus> Status() const noexcept;

    /// \brief Determines the type and attributes of the filesystem object identified by p as if by POSIX stat
    /// (symlinks are NOT followed to their targets).
    score::Result<FileStatus> SymlinkStatus() const noexcept;

    friend bool operator==(const DirectoryEntry& l, const DirectoryEntry& r) noexcept;
    friend bool operator!=(const DirectoryEntry& l, const DirectoryEntry& r) noexcept;

  private:
    Path current_entry_;
};

bool operator==(const DirectoryEntry& l, const DirectoryEntry& r) noexcept;
bool operator!=(const DirectoryEntry& l, const DirectoryEntry& r) noexcept;

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_DIRECTORY_ENTRY_H
