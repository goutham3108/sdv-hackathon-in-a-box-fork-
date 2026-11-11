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
#include "score/filesystem/filestream/file_factory_fake.h"
#include "score/filesystem/filestream/bad_string_stream_collection.h"
#include "score/filesystem/filestream/simple_string_stream_collection.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(FileFactoryFakeTest, CanOpenStringStream)
{
    SimpleStringStreamCollection collection{};
    FileFactoryFake unit{collection};
    {
        auto file_result = unit.Open("/tmp/some_file", std::ios_base::out);

        ASSERT_TRUE(file_result.has_value());

        *file_result.value() << "Foo";
    }

    const auto& file = unit.Get("/tmp/some_file");
    EXPECT_EQ(file.str(), "Foo");
}

TEST(FileFactoryFakeTest, FailedToOpenStringStream)
{
    BadStringStreamCollection bad_collection{};  // LCOV_EXCL_LINE
    FileFactoryFake unit{bad_collection};

    const auto file_result = unit.Open("/tmp/some_file", std::ios_base::out);

    ASSERT_FALSE(file_result.has_value());
    EXPECT_EQ(static_cast<ErrorCode>(*file_result.error()), ErrorCode::kCouldNotOpenFileStream);
}

}  // namespace
}  // namespace filesystem
}  // namespace score
