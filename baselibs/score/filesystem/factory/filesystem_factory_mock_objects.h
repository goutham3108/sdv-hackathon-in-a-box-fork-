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
#ifndef SCORE_LIB_FILESYSTEM_FACTORY_FILESYSTEM_FACTORY_MOCK_OBJECTS_H
#define SCORE_LIB_FILESYSTEM_FACTORY_FILESYSTEM_FACTORY_MOCK_OBJECTS_H

#include "score/filesystem/factory/filesystem_factory_mock.h"

#include "score/filesystem/file_utils/file_utils_mock.h"
#include "score/filesystem/filestream/file_factory_mock.h"
#include "score/filesystem/standard_filesystem_mock.h"

#include <gmock/gmock.h>

namespace score
{
namespace filesystem
{

class FilesystemFactoryMockObjects : public FilesystemFactoryMock
{
  public:
    FilesystemFactoryMockObjects();

    Filesystem MockObjectsCreateInstance() const noexcept;

    StandardFilesystemMock& GetStandard() const noexcept;
    FileUtilsMock& GetUtils() const noexcept;
    FileFactoryMock& GetStreams() const noexcept;

  private:
    std::shared_ptr<StandardFilesystemMock> standard_{std::make_shared<StandardFilesystemMock>()};
    std::shared_ptr<FileUtilsMock> utils_{std::make_shared<FileUtilsMock>()};
    std::shared_ptr<FileFactoryMock> streams_{std::make_shared<FileFactoryMock>()};
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FACTORY_FILESYSTEM_FACTORY_MOCK_OBJECTS_H
