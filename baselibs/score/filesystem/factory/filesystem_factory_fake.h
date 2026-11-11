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
#ifndef SCORE_LIB_FILESYSTEM_FACTORY_FILESYSTEM_FACTORY_FAKE_H
#define SCORE_LIB_FILESYSTEM_FACTORY_FILESYSTEM_FACTORY_FAKE_H

#include "score/filesystem/factory/filesystem_factory_mock.h"

#include "score/filesystem/file_utils/file_utils_fake.h"
#include "score/filesystem/filestream/file_factory_fake.h"
#include "score/filesystem/standard_filesystem_fake.h"

#include <gmock/gmock.h>

namespace score
{
namespace filesystem
{

class FilesystemFactoryFake : public FilesystemFactoryMock
{
  public:
    FilesystemFactoryFake();

    Filesystem FakeCreateInstance() const noexcept;

    StandardFilesystemFake& GetStandard() const noexcept;
    FileUtilsFake& GetUtils() const noexcept;
    FileFactoryFake& GetStreams() const noexcept;

  private:
    std::shared_ptr<StandardFilesystemFake> standard_{std::make_shared<StandardFilesystemFake>()};
    std::shared_ptr<FileFactoryFake> streams_{std::make_shared<FileFactoryFake>(*standard_)};
    std::shared_ptr<FileUtilsFake> utils_{std::make_shared<FileUtilsFake>(*standard_, *streams_)};
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FACTORY_FILESYSTEM_FACTORY_FAKE_H
