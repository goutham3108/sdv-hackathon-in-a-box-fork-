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
#ifndef SCORE_LIB_FILESYSTEM_DETAILS_TEST_HELPER_H
#define SCORE_LIB_FILESYSTEM_DETAILS_TEST_HELPER_H

#include "score/filesystem/path.h"
#include "score/os/dirent.h"

#include <string>

namespace score
{
namespace filesystem
{
namespace test
{

union DirentWithCorrectSize
{
    dirent entry;
    char name_buffer[280]{};  // As per POSIX standard dirent.d_name is of unspecified size. On Linux it is hardcoded
                              // 256. To achieve interoperability we provide a custom length buffer (as proposed by QNX
                              // doc).
                              // What is also important that it is not enough just to add the padding after the real
                              // dirent but actually unionize it will spaced enough buffer. Magick number 280 is chosen
                              // to fit in MAX_PATH together with dirent data and offsets. It should be more than enough
                              // for unit testing purposes.
};

Path InitTempDirectoryFor(const std::string& test_name);

}  // namespace test
}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_DETAILS_TEST_HELPER_H
