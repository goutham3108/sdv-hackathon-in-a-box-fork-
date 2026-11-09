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
#include "score/filesystem/factory/filesystem_factory_mock_objects.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(FilesystemFactoryMockObjectsTest, InstanceResolution)
{
    FilesystemFactoryMockObjects unit;

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_NE(fs.standard.get(), nullptr);
    EXPECT_NE(fs.utils.get(), nullptr);
    EXPECT_NE(fs.streams.get(), nullptr);
}

TEST(FilesystemFactoryMockObjectsTest, SameInstanceForStandard)
{
    FilesystemFactoryMockObjects unit;
    StandardFilesystemMock* mock = &unit.GetStandard();
    IStandardFilesystem* expected{mock};

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_EQ(fs.standard.get(), expected);
}

TEST(FilesystemFactoryMockObjectsTest, SameInstanceForUtils)
{
    FilesystemFactoryMockObjects unit;
    FileUtilsMock* mock = &unit.GetUtils();
    IFileUtils* expected{mock};

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_EQ(fs.utils.get(), expected);
}

TEST(FilesystemFactoryMockObjectsTest, SameInstanceForStreams)
{
    FilesystemFactoryMockObjects unit;
    FileFactoryMock* mock = &unit.GetStreams();
    IFileFactory* expected{mock};

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_EQ(fs.streams.get(), expected);
}

}  // namespace
}  // namespace filesystem
}  // namespace score
