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
#include "score/filesystem/details/test_helper.h"

#include <score/assert.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

namespace score
{
namespace filesystem
{
namespace test
{

Path InitTempDirectoryFor(const std::string& test_name)
{
    std::string tmp_dir_template = "/tmp/" + test_name + ".XXXXXX";
    auto* result = ::mkdtemp(&tmp_dir_template.front());  // we are force to provide a writeable string by the POSIX API
    if (result != nullptr)
    {
        return result;
    }

    // we have to execute unit tests on target. our target does not support directories in /tmp, thus we use the
    // only other writeable place.
    ::mkdir("tmp", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    std::string tmp_dir_template_on_target = "tmp/" + test_name + ".XXXXXX";
    auto* result_on_target = ::mkdtemp(&tmp_dir_template_on_target.front());
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG_MESSAGE(result_on_target != nullptr, "");

    // all tests assume 'tmp' path to be in canonical absolute format, so apply realpath()
    // after dir is created
    // TODO: not sure anybody will free() or re-use the buffer allocated by realpath(),
    // but even if it is a leak it should not be critical for unit test
    return ::realpath(result_on_target, nullptr);
}

}  // namespace test
}  // namespace filesystem
}  // namespace score
