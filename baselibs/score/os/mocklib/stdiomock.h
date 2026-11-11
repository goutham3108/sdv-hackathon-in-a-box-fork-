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
#ifndef SCORE_LIB_OS_MOCKLIB_STDIOMOCK_H
#define SCORE_LIB_OS_MOCKLIB_STDIOMOCK_H

#include "score/os/stdio.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class StdioMock : public Stdio
{
  public:
    MOCK_METHOD((score::cpp::expected<FILE*, Error>), fopen, (const char*, const char*), (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>, fclose, (FILE*), (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>, remove, (const char*), (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>, rename, (const char*, const char*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<FILE*, Error>), popen, (const char*, const char*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>), pclose, (FILE*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>), fileno, (FILE*), (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_STDIOMOCK_H
