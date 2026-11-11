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
#include "score/os/utils/qnx/resource_manager/include/error.h"

#include <gtest/gtest.h>

namespace
{
using ErrorCode = score::os::ErrorCode;

struct ResourceManagerErrorDomainTest : ::testing::Test
{
    void errorMessageTesting(ErrorCode code, std::string_view message)
    {
        EXPECT_EQ(score::os::MakeError(code).Message().compare(message), 0);
    }
};

TEST_F(ResourceManagerErrorDomainTest, checkErrorCodesMessages)
{
    errorMessageTesting(ErrorCode::kInvalidArgument, strerror(static_cast<uint32_t>(ErrorCode::kInvalidArgument)));
    errorMessageTesting(ErrorCode::kIllegalSeek, strerror(static_cast<uint32_t>(ErrorCode::kIllegalSeek)));
    errorMessageTesting(static_cast<ErrorCode>(0), "Unknown error.");
}

}  // namespace
