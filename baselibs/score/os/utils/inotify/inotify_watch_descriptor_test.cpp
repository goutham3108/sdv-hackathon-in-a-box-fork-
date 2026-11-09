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
#include "score/os/utils/inotify/inotify_watch_descriptor.h"

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{

class InotifyWatchDescriptorTest : public ::testing::Test
{
};

TEST_F(InotifyWatchDescriptorTest, CanGetUnderylingDescriptor)
{
    std::int32_t watch_descriptor{24};
    InotifyWatchDescriptor inotify_watch_descriptor{watch_descriptor};
    EXPECT_EQ(inotify_watch_descriptor.GetUnderlying(), watch_descriptor);
}

TEST_F(InotifyWatchDescriptorTest, EqualityOperatorComparesEqual)
{
    std::int32_t watch_descriptor{24};
    InotifyWatchDescriptor inotify_watch_descriptor_lhs{watch_descriptor};
    InotifyWatchDescriptor inotify_watch_descriptor_rhs{watch_descriptor};
    EXPECT_TRUE(inotify_watch_descriptor_lhs == inotify_watch_descriptor_rhs);
}

TEST_F(InotifyWatchDescriptorTest, EqualityOperatorComparesInequal)
{
    std::int32_t watch_descriptor_lhs{24};
    InotifyWatchDescriptor inotify_watch_descriptor_lhs{watch_descriptor_lhs};
    std::int32_t watch_descriptor_rhs{23};
    InotifyWatchDescriptor inotify_watch_descriptor_rhs{watch_descriptor_rhs};
    EXPECT_FALSE(inotify_watch_descriptor_lhs == inotify_watch_descriptor_rhs);
}

TEST_F(InotifyWatchDescriptorTest, InequalityOperatorComparesInequal)
{
    std::int32_t watch_descriptor{24};
    InotifyWatchDescriptor inotify_watch_descriptor_lhs{watch_descriptor};
    InotifyWatchDescriptor inotify_watch_descriptor_rhs{watch_descriptor};
    EXPECT_FALSE(inotify_watch_descriptor_lhs != inotify_watch_descriptor_rhs);
}

TEST_F(InotifyWatchDescriptorTest, InequalityOperatorComparesEqual)
{
    std::int32_t watch_descriptor_lhs{24};
    InotifyWatchDescriptor inotify_watch_descriptor_lhs{watch_descriptor_lhs};
    std::int32_t watch_descriptor_rhs{23};
    InotifyWatchDescriptor inotify_watch_descriptor_rhs{watch_descriptor_rhs};
    EXPECT_TRUE(inotify_watch_descriptor_lhs != inotify_watch_descriptor_rhs);
}

TEST_F(InotifyWatchDescriptorTest, HashOfSameObjectsDoesNotDiffer)
{
    std::int32_t watch_descriptor_lhs{23};
    InotifyWatchDescriptor inotify_watch_descriptor_lhs{watch_descriptor_lhs};
    const auto lhs_hash = std::hash<InotifyWatchDescriptor>()(inotify_watch_descriptor_lhs);

    std::int32_t watch_descriptor_rhs{23};
    InotifyWatchDescriptor inotify_watch_descriptor_rhs{watch_descriptor_rhs};
    const auto rhs_hash = std::hash<InotifyWatchDescriptor>()(inotify_watch_descriptor_rhs);
    EXPECT_EQ(lhs_hash, rhs_hash);
}

TEST_F(InotifyWatchDescriptorTest, HashOfDifferentObjectsDiffers)
{
    std::int32_t watch_descriptor_lhs{24};
    InotifyWatchDescriptor inotify_watch_descriptor_lhs{watch_descriptor_lhs};
    const auto lhs_hash = std::hash<InotifyWatchDescriptor>()(inotify_watch_descriptor_lhs);

    std::int32_t watch_descriptor_rhs{23};
    InotifyWatchDescriptor inotify_watch_descriptor_rhs{watch_descriptor_rhs};
    const auto rhs_hash = std::hash<InotifyWatchDescriptor>()(inotify_watch_descriptor_rhs);
    EXPECT_NE(lhs_hash, rhs_hash);
}

}  // namespace
}  // namespace os
}  // namespace score
