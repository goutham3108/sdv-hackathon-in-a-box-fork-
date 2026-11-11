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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_DEVCTL_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_DEVCTL_H

#include "score/os/qnx/devctl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

class MockDevctl : public Devctl
{
  public:
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                devctl,
                (const std::int32_t, const std::int32_t, void* const, const std::size_t, std::int32_t* const),
                (const, noexcept, override));

    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                devctlv,
                (const std::int32_t,
                 const std::int32_t,
                 const std::int32_t,
                 const std::int32_t,
                 const iovec*,
                 const iovec*,
                 std::int32_t* const),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_DEVCTL_H
