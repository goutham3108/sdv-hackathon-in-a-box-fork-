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
#ifndef SCORE_LIB_OS_UTILS_MOCKLIB_PATHMOCK_H
#define SCORE_LIB_OS_UTILS_MOCKLIB_PATHMOCK_H

#include "score/os/errno.h"
#include "score/os/utils/path.h"

#include <gmock/gmock.h>
#include <string>

namespace score
{
namespace os
{

class PathMock : public Path
{
  public:
    MOCK_METHOD((std::string), get_base_name, (const std::string& path), (const, noexcept, override));
    MOCK_METHOD((std::string), get_parent_dir, (const std::string& path), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::string, score::os::Error>), get_exec_path, (), (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_MOCKLIB_PATHMOCK_H
