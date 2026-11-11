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
#include "score/filesystem/file_utils/file_test_utils.h"
#include "score/filesystem/error.h"
#include <sys/stat.h>

namespace score::filesystem
{

Result<Path> FileTestUtils::GetTempDirectory()
{
    // First, check whether we have TEST_TMPDIR (which is set by Bazel)
    // coverity[autosar_cpp14_m18_0_3_violation] Test code, not a problem to use getenv.
    const auto* env_tmpdir = std::getenv("TEST_TMPDIR");
    if (env_tmpdir == nullptr)
    {
        // Otherwise, check whether we might be on a target
        //
        // This is a poor-man's test whether we're being run on real hardware
        // If /persistent happens to exist anywhere else, we just assume that reusing this directory is also ok in
        // case we're allowed to create a directory here.
        //
        // We're using pure POSIX here since the stdc++ filesystem lib doesn't seem to exist on the target toolchain,
        // and we're testing our filesystem library here which makes it brittle to use other parts of itself to test
        // it.
        struct ::stat statbuf{};
        if (::stat("/persistent", &statbuf) == 0)  // If "/persistent" exists...
        {
            if (statbuf.st_mode & S_IFDIR)  // ...and is a directory
            {
                if (::stat("/persistent/tmp", &statbuf) != 0)  // ...and if "/persistent/tmp" does not exist
                {
                    // ...then create it
                    if (::mkdir("/persistent/tmp", 0777) != 0)
                    {
                        return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory,
                                              "Cannot create temporary directory on /persistent");
                    }
                }
                env_tmpdir = "/persistent/tmp";
            }
            else
            {
                return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory,
                                      "/persistent is not a directory. What gives?!");
            }
        }

        // As a last resort, use TMPDIR or just /tmp
        if (env_tmpdir == nullptr)
        {
            env_tmpdir = std::getenv("TMPDIR");
        }
        if (env_tmpdir == nullptr)
        {
            env_tmpdir = "/tmp";
        }
    }

    return {env_tmpdir};
}

}  // namespace score::filesystem
