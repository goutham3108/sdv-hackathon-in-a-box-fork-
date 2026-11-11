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
#ifndef SCORE_LIB_OS_MOCKLIB_STRING_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_STRING_MOCK_H

#include "score/os/string.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class StringMock : public String
{
  public:
    MOCK_METHOD(void*, memcpy, (void*, const void*, const std::size_t), (const, noexcept, override));
    MOCK_METHOD(char*, strerror, (const std::int32_t), (const, noexcept, override));
    MOCK_METHOD(void*, memset, (void*, std::int32_t, const std::size_t), (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_STRING_MOCK_H
