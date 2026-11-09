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
#ifndef SCORE_LIB_FILESYSTEM_FILESYSTEM_MOCK_H
#define SCORE_LIB_FILESYSTEM_FILESYSTEM_MOCK_H

// Contains fake and mock headers for use in unit tests of components that use the filesystem library

#include "score/filesystem/filesystem.h"
// mock/fake for IStandardFilesystem
#include "score/filesystem/standard_filesystem_fake.h"
#include "score/filesystem/standard_filesystem_mock.h"
// fake for directory iterators
#include "score/filesystem/iterator/dirent_fake.h"
// mock/fake for IFileFactory
#include "score/filesystem/filestream/file_factory_fake.h"
#include "score/filesystem/filestream/file_factory_mock.h"
#include "score/filesystem/filestream/simple_string_stream_collection.h"
// mock for IFileUtils
#include "score/filesystem/file_utils/file_utils_mock.h"

#include <gmock/gmock.h>

namespace score::filesystem
{

template <template <class> typename MockQualifier>
inline score::filesystem::Filesystem CreateMockFileSystem()
{
    return {std::make_shared<MockQualifier<StandardFilesystemMock>>(),
            std::make_shared<MockQualifier<FileUtilsMock>>(),
            std::make_shared<MockQualifier<FileFactoryMock>>()};
}

score::filesystem::Filesystem CreateMockFileSystem();

}  // namespace score::filesystem

#endif  // SCORE_LIB_FILESYSTEM_FILESYSTEM_MOCK_H
