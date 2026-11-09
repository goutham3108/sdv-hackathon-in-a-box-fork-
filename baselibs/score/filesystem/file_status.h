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
#ifndef SCORE_LIB_FILESYSTEM_FILE_STATUS_H
#define SCORE_LIB_FILESYSTEM_FILE_STATUS_H

#include "score/os/stat.h"

#include <cstdint>

namespace score
{
namespace filesystem
{

using Perms = score::os::Stat::Mode;

enum class FileType : std::int32_t
{
    kNone,
    kNotFound,
    kRegular,
    kDirectory,
    kSymlink,
    kBlock,
    kCharacter,
    kFifo,
    kSocket,
    kUnknown
};

/// \brief Stores information about the type and permissions of a file.
class FileStatus final
{
  public:
    /// \brief Constructs a new file_status object (with FileType::kNone)
    explicit FileStatus() noexcept;

    /// \brief Initializes the file status object with type as type and permissions as permissions.
    explicit FileStatus(const FileType, const Perms = Perms::kUnknown) noexcept;

    FileStatus(const FileStatus&) noexcept;
    FileStatus(FileStatus&&) noexcept;

    FileStatus& operator=(const FileStatus&) noexcept;
    FileStatus& operator=(FileStatus&&) noexcept;

    ~FileStatus() noexcept;

    /// \brief Returns the type information
    FileType Type() const noexcept;

    /// \brief Sets the file type
    void Type(const FileType) noexcept;

    /// \brief Returns the file permissions information
    Perms Permissions() const noexcept;

    /// \brief Sets the file permissions
    void Permissions(const Perms) noexcept;

    friend bool operator==(const FileStatus&, const FileStatus&) noexcept;

  private:
    FileType type_;
    Perms permissions_;
};

bool operator==(const FileStatus&, const FileStatus&) noexcept;

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FILE_STATUS_H
