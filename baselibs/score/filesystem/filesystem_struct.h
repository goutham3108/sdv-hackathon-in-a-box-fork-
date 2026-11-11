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
#ifndef SCORE_LIB_FILESYSTEM_FILESYSTEM_STRUCT_H
#define SCORE_LIB_FILESYSTEM_FILESYSTEM_STRUCT_H

#include "score/filesystem/file_utils/i_file_utils.h"
#include "score/filesystem/filestream/i_file_factory.h"
#include "score/filesystem/i_standard_filesystem.h"

#include <memory>

namespace score
{
namespace filesystem
{

struct Filesystem
{
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::shared_ptr<IStandardFilesystem> standard;
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::shared_ptr<IFileUtils> utils;
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::shared_ptr<IFileFactory> streams;
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FILESYSTEM_STRUCT_H
