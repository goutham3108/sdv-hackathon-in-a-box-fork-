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
#include "score/os/qnx/qnx_version.h"
#include <gtest/gtest.h>

class QnxVersionTest : public ::testing::Test
{
};

TEST(QnxVersionTest, _SPPQnxVersion_IsDefinedForQnxOS)
{
    EXPECT_NE(_SPP_QNX_VERSION_, 0u);
}

TEST(QnxVersionTest, _SPPQnxVersion_MatchesQnxMacro)
{
#if __QNX__ >= 800  // QNX 8 or later
    EXPECT_EQ(_SPP_QNX_VERSION_, __QNX__);
#else
    EXPECT_EQ(_SPP_QNX_VERSION_, _NTO_VERSION);
#endif
}

TEST(QnxVersionTest, _SPPQnxVersion_IsWithinValidRange)
{
    EXPECT_GE(_SPP_QNX_VERSION_, 700u);   // Assuming QNX version is at least 7.0
    EXPECT_LT(_SPP_QNX_VERSION_, 1000u);  // Assuming QNX version is less than 10.0
}

#if __QNX__ >= 800  // QNX 8 or later
TEST(QnxVersionTest, SPPQnxVersionIsQnx8OrHigher)
{
    EXPECT_GE(_SPP_QNX_VERSION_, 800u);
}
#else
TEST(QnxVersionTest, _SPPQnxVersion_IsLowerThanQnx8)
{
    EXPECT_LT(_SPP_QNX_VERSION_, 800u);
}
#endif
