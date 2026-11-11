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
#include "score/filesystem/file_utils/i_file_utils.h"

#include "score/filesystem/file_utils/file_utils.h"
#include "score/filesystem/filestream/i_file_factory.h"
#include "score/filesystem/i_standard_filesystem.h"

namespace score
{
namespace filesystem
{

IFileUtils& IFileUtils::instance() noexcept
{
    // singleton
    // coverity[autosar_cpp14_a3_3_2_violation]
    static FileUtils instance{IStandardFilesystem::instance(),
                              IFileFactory::instance()};  // LCOV_EXCL_BR_LINE caused by exceptions
    return select_instance(instance);
}

IFileUtils::~IFileUtils() noexcept = default;

}  // namespace filesystem
}  // namespace score
