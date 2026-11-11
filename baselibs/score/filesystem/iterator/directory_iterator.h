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
#ifndef SCORE_LIB_FILESYSTEM_DIRECTORY_ITERATOR_H
#define SCORE_LIB_FILESYSTEM_DIRECTORY_ITERATOR_H

#include "score/filesystem/iterator/directory_entry.h"
#include "score/filesystem/path.h"
#include "score/os/errno.h"

#include <dirent.h>
#include <cstddef>
#include <memory>
#include <optional>

namespace score
{
namespace filesystem
{

enum class DirectoryOptions : std::uint16_t
{
    kNone = 0,  // (Default)
    kFollowDirectorySymlink = 1,
    // kSkipPermissionDenied = 2,  // not supported, feel free to implement
};

/// @brief DirectoryIterator is a `LegacyInputIterator` that iterates over the DirectoryEntry elements of a directory
/// (but does not visit the subdirectories). The iteration order is unspecified, except that each directory entry is
/// visited only once. The special pathnames `dot` and `dot-dot` are skipped.
///
/// If the DirectoryIterator reports an error or is advanced past the last directory entry, it becomes equal to the
/// default-constructed iterator, also known as the end iterator. Two end iterators are always equal, dereferencing or
/// incrementing the end iterator is undefined behavior.
///
/// If a file or a directory is deleted or added to the directory tree after the DirectoryIterator has been created, it
/// is unspecified whether the change would be observed through the iterator.
///
/// @note Not all features (e.g. DirectoryOptions::kSkipPermissionDenied) are implemented.
//  If they are needed then feel free to add them.
///
/// @see https://en.cppreference.com/w/cpp/filesystem/directory_iterator
class DirectoryIterator final  // LCOV_EXCL_BR_LINE false positive
{
  public:
    using value_type = DirectoryEntry;
    using difference_type = std::ptrdiff_t;
    using pointer = const DirectoryEntry*;
    using reference = const DirectoryEntry&;
    using iterator_category = std::input_iterator_tag;

    /// @brief Constructs the end iterator.
    DirectoryIterator() noexcept;

    /// @brief Constructs a directory iterator that refers to the first directory entry of a directory identified
    explicit DirectoryIterator(const Path& path,
                               const DirectoryOptions directory_options = DirectoryOptions::kNone) noexcept;
    DirectoryIterator(const DirectoryIterator&) noexcept;
    DirectoryIterator(DirectoryIterator&&) noexcept;
    DirectoryIterator& operator=(const DirectoryIterator&) noexcept;
    DirectoryIterator& operator=(DirectoryIterator&&) noexcept;
    ~DirectoryIterator() noexcept;

    /// @brief Accesses the pointed-to DirectoryEntry
    reference operator*() const noexcept;

    /// @brief Accesses the pointed-to DirectoryEntry
    pointer operator->() const noexcept;

    /// @brief Advances the iterator to the next entry. Invalidates all copies of the previous value of *this.
    DirectoryIterator& operator++() noexcept;

    /// @brief Checks whether an error occurred while iterating through a directory
    /// @note This is an extension to the C++ std::filesystem version, needed from proper error reporting.
    bool HasValue() const noexcept;

    /// @brief Returns the error that occurred while iterating through a directory
    /// @note This is an extension to the C++ std::filesystem version, needed from proper error reporting.
    score::os::Error Error() const noexcept;

    friend bool operator==(const DirectoryIterator& l, const DirectoryIterator& r) noexcept;
    friend bool operator!=(const DirectoryIterator& l, const DirectoryIterator& r) noexcept;

  private:
    /// @brief Holds the shared-state of DirectoryIterators. Since it is an implementation detail (and shall not be used
    /// by any occurrence outside), we hide it as inner-class.
    class Directory final
    {
      public:
        explicit Directory(const Path&) noexcept;
        Directory(const Directory&) = delete;
        Directory(Directory&&) = delete;
        Directory& operator=(const Directory&) = delete;
        Directory& operator=(Directory&&) = delete;
        Directory& operator++();
        const DirectoryEntry& CurrentEntry() const noexcept;
        bool HasError() const noexcept;
        score::os::Error GetError() const noexcept;
        void SetError(score::os::Error error) noexcept;
        ~Directory() noexcept;

      private:
        Path path_;
        score::os::Result<DIR*> posix_directory_;
        DirectoryEntry current_entry_;
        std::optional<score::os::Error> error_;
    };

    std::shared_ptr<Directory> directory_;
};

/// @brief Enable the use of DirectoryIterator with range-based for loops
DirectoryIterator begin(const DirectoryIterator&) noexcept;

/// @brief Enable the use of DirectoryIterator with range-based for loops
DirectoryIterator end(const DirectoryIterator&) noexcept;

bool operator==(const DirectoryIterator& l, const DirectoryIterator& r) noexcept;
bool operator!=(const DirectoryIterator& l, const DirectoryIterator& r) noexcept;

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_DIRECTORY_ITERATOR_H
