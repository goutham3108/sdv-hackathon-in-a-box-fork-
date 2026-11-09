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
#include "score/filesystem/file_status.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(FileStatus, DefaultConstructedAsNone)
{
    FileStatus unit{};

    EXPECT_EQ(unit.Type(), FileType::kNone);
    EXPECT_EQ(unit.Permissions(), Perms::kUnknown);
}

TEST(FileStatus, ConstructWithTypeAndPerms)
{
    FileStatus unit{FileType::kRegular, Perms::kReadOthers};

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, CanCopyConstruct)
{
    const FileStatus other{FileType::kRegular, Perms::kReadOthers};

    FileStatus unit{other};

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, CanMoveConstruct)
{
    FileStatus other{FileType::kRegular, Perms::kReadOthers};

    FileStatus unit{std::move(other)};

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, CanCopyAssign)
{
    const FileStatus other{FileType::kRegular, Perms::kReadOthers};

    FileStatus unit = other;

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, CanMoveAssign)
{
    FileStatus other{FileType::kRegular, Perms::kReadOthers};

    FileStatus unit = std::move(other);

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, SetType)
{
    FileStatus unit{FileType::kRegular, Perms::kReadOthers};

    unit.Type(FileType::kDirectory);

    EXPECT_EQ(unit.Type(), FileType::kDirectory);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, SetPermissions)
{
    FileStatus unit{FileType::kRegular, Perms::kReadOthers};

    unit.Permissions(Perms::kWriteOthers);

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kWriteOthers);
}

TEST(FileStatus, EqualOperator)
{
    FileStatus unit{FileType::kRegular, Perms::kReadOthers};
    FileStatus same{FileType::kRegular, Perms::kReadOthers};
    FileStatus other_file_type{FileType::kSocket, Perms::kReadOthers};
    FileStatus other_permission{FileType::kRegular, Perms::kWriteOthers};

    EXPECT_TRUE(unit == same);
    EXPECT_FALSE(unit == other_file_type);
    EXPECT_FALSE(unit == other_permission);
}

}  // namespace
}  // namespace filesystem
}  // namespace score
