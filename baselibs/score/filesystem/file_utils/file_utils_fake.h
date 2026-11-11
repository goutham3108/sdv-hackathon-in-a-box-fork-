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
#ifndef SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_UTILS_FAKE_H
#define SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_UTILS_FAKE_H

#include "score/filesystem/file_utils/file_utils.h"
#include "score/filesystem/file_utils/file_utils_mock.h"
#include "score/filesystem/filestream/i_file_factory.h"
#include "score/filesystem/i_standard_filesystem.h"

namespace score
{
namespace filesystem
{

class FileUtilsFake : public FileUtilsMock
{
  public:
    FileUtilsFake(IStandardFilesystem& standard_filesystem, IFileFactory& file_factory) noexcept;

  private:
    FileUtils file_utils_;
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_UTILS_FAKE_H
