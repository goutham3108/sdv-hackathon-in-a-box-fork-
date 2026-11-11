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
#ifndef SCORE_LIB_OS_MOCKLIB_GETOPTMOCK_H
#define SCORE_LIB_OS_MOCKLIB_GETOPTMOCK_H

#include "score/os/getopt.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class GetoptMock : public Getopt
{
  public:
    MOCK_METHOD(std::int32_t,
                getopt,
                (const std::int32_t, char* const argv[], const char*),
                (const, noexcept, override));
    MOCK_METHOD(std::int32_t, getoptopt, (), (const, noexcept, override));
    MOCK_METHOD(std::int32_t, getoptind, (), (const, noexcept, override));
    MOCK_METHOD(std::int32_t, getopterr, (), (const, noexcept, override));
    MOCK_METHOD(std::int32_t,
                getopt_long,
                (std::int32_t, char* const argv[], const char*, const option*, std::int32_t*),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_GETOPTMOCK_H
