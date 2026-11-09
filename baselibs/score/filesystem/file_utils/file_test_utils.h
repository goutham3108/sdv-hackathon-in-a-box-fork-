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
#ifndef SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_TEST_UTILS_H
#define SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_TEST_UTILS_H

#include "score/filesystem/path.h"
#include "score/result/result.h"

namespace score::filesystem
{

class FileTestUtils
{
  public:
    /// Returns a directory that is suitable for temporary files created during a test run.
    ///
    /// The main purpose of this method is to support tests that require support of file system operations that aren't
    /// supported on the QNX /tmp directory. One such operation is the rename syscall. This method will therefore work
    /// like this:
    /// * Check whether TEST_TMPDIR is set. In this case, it gets used.
    /// * Check whether /persistent exists and is a directory. If so, a subdirectory named "tmp" is created and
    ///   /persistent/tmp is returned.
    /// * Check whether TMPDIR is set. If so, it gets used.
    /// * Otherwise, /tmp is used.
    ///
    /// \return A suitable temporary directory path, or an error.
    static Result<Path> GetTempDirectory();
};

}  // namespace score::filesystem

#endif  // SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_TEST_UTILS_H
