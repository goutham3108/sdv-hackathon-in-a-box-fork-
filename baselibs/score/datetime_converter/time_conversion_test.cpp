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
#include "score/datetime_converter/time_conversion.h"

#include <gtest/gtest.h>
#include <cstdint>

TEST(DurationToTimespec, Millisecond)
{
    struct timespec ts = score::common::duration_to_timespec(std::chrono::milliseconds(1));

    EXPECT_EQ(ts.tv_sec, 0);
    EXPECT_EQ(ts.tv_nsec, 1000000);
}

TEST(DurationToTimespec, Hour)
{
    struct timespec ts = score::common::duration_to_timespec(std::chrono::hours(1));

    EXPECT_EQ(ts.tv_sec, 3600);
    EXPECT_EQ(ts.tv_nsec, 0);
}

TEST(MilisecToTimesec, test)
{
    std::chrono::time_point<std::chrono::system_clock> t(std::chrono::seconds(4));
    struct timespec ts = score::common::timeout_in_timespec(std::chrono::milliseconds(1), t);

    EXPECT_EQ(ts.tv_sec, 4);
    EXPECT_EQ(ts.tv_nsec, 1000000);
}
