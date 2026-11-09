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
#ifndef SCORE_LIB_FILESYSTEM_ITERATOR_DIRENT_FAKE_H
#define SCORE_LIB_FILESYSTEM_ITERATOR_DIRENT_FAKE_H

#include "score/filesystem/path.h"
#include "score/os/mocklib/mock_dirent.h"

#include <list>
#include <unordered_map>
#include <vector>

namespace score
{
namespace filesystem
{

class StandardFilesystemFake;

/// @brief DirentFake is a fake object, simplified version of Dirent.
///
/// This class works with and requires StandardFilesystemFake instance.
/// This class is implemented for fake directory iterators
/// The usage examples can be found in the tests:
/// DirentFakeFixture.DirectoryIterator and DirentFakeFixture.RecursiveDirectoryIterator.
/// The 'scandir' function is not implemented because this function is not used in directory iterators.
/// @note No need to create DirentFake object or MockGuard<DirentFake>. Use MockGuard<StandardFilesystemFake> instead.
class DirentFake : public score::os::MockDirent
{
  public:
    /// @brief Constructs an fake object: stores the reference to StandardFilesystemFake object.
    DirentFake(StandardFilesystemFake& filesystem);

  private:
    score::cpp::expected<DIR*, score::os::Error> FakeOpendir(const char* name) noexcept;
    score::cpp::expected<struct dirent*, score::os::Error> FakeReaddir(DIR* dirp) noexcept;
    score::cpp::expected_blank<score::os::Error> FakeClosedir(DIR* dirp) noexcept;

    struct DirStream
    {
        using Filelist = std::list<std::string>;
        Path path_;
        std::vector<uint8_t> dirent_buffer_;
        Filelist filelist_;
        Filelist::iterator current_;
    };

    StandardFilesystemFake& filesystem_;
    std::unordered_map<DIR*, DirStream> dir_streams_;
    std::size_t next_id_;
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_ITERATOR_DIRENT_FAKE_H
