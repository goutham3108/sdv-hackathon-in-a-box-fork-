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
#include <gtest/gtest.h>

#include "score/os/version.h"

class VersionComparisonMacroTest
    : public ::testing::TestWithParam<std::tuple<std::uint32_t, std::uint32_t, std::uint32_t, bool, bool>>
{
  protected:
    std::uint32_t version, major, minor;
    bool expectedGTE, expectedLT;

    void SetUp() override
    {
        std::tie(version, major, minor, expectedGTE, expectedLT) = GetParam();
    }
};

TEST_P(VersionComparisonMacroTest, SppOsVersionGTE)
{
    EXPECT_EQ(SPP_OS_VERSION_GTE(version, major, minor), expectedGTE);
}

TEST_P(VersionComparisonMacroTest, SppOsVersionLT)
{
    EXPECT_EQ(SPP_OS_VERSION_LT(version, major, minor), expectedLT);
}

INSTANTIATE_TEST_SUITE_P(VersionComparisonMacroTests,
                         VersionComparisonMacroTest,
                         ::testing::Values(
                             // Test cases: {version, major, minor, expectedGTE, expectedLT}
                             std::make_tuple(700, 7, 0, true, false),     // Exact match
                             std::make_tuple(710, 7, 10, true, false),    // Exact match
                             std::make_tuple(800, 7, 0, true, false),     // Greater major
                             std::make_tuple(801, 7, 99, true, false),    // Greater major and minor
                             std::make_tuple(711, 7, 10, true, false),    // Greater minor
                             std::make_tuple(799, 7, 98, true, false),    // Greater minor
                             std::make_tuple(699, 7, 0, false, true),     // Less major
                             std::make_tuple(709, 7, 10, false, true),    // Less minor
                             std::make_tuple(0, 0, 0, true, false),       // Edge case: lowest version
                             std::make_tuple(9999, 99, 99, true, false),  // Edge case: highest version
                             std::make_tuple(100, 1, 0, true, false),     // Boundary value
                             std::make_tuple(99, 1, 0, false, true)       // Boundary value
                             ));

class QnxVersionTest : public ::testing::Test
{
};

#ifdef __QNX__
TEST_F(QnxVersionTest, SppOsQnxVersionMacroIsNotZeroInQnxEnvironment)
{
    EXPECT_GT(SPP_OS_QNX_VERSION, 0u);
}

TEST_F(QnxVersionTest, SppOsQnx8MacroDefinitionIsWithinValidRange)
{
#ifdef SPP_OS_QNX8
    EXPECT_GE(SPP_OS_QNX_VERSION, 800);
    EXPECT_LT(SPP_OS_QNX_VERSION, 900);
#else
    EXPECT_TRUE(SPP_OS_QNX_VERSION < 800 || SPP_OS_QNX_VERSION >= 900);
#endif
}

TEST_F(QnxVersionTest, SppOsQnx7MacroDefinitionIsWithinValidRange)
{
#ifdef SPP_OS_QNX7
    EXPECT_GE(SPP_OS_QNX_VERSION, 700);
    EXPECT_LT(SPP_OS_QNX_VERSION, 800);
#else
    EXPECT_TRUE(SPP_OS_QNX_VERSION < 700 || SPP_OS_QNX_VERSION >= 800);
#endif
}
#endif

#ifndef __QNX__
TEST_F(QnxVersionTest, SppOsQnx8NotDefinedInNonQnxEnvironment)
{
    EXPECT_EQ(SPP_OS_QNX_VERSION, 0u);
#ifdef SPP_OS_QNX8
    ADD_FAILURE() << "SPP_OS_QNX8 should not be defined in non-QNX environment";
#endif
}

TEST_F(QnxVersionTest, SppOsQnx7NotDefinedInNonQnxEnvironment)
{
    EXPECT_EQ(SPP_OS_QNX_VERSION, 0u);
#ifdef SPP_OS_QNX7
    ADD_FAILURE() << "SPP_OS_QNX7 should not be defined in non-QNX environment";
#endif
}
#endif
