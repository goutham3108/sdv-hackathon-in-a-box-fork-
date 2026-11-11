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
#include "score/filesystem/filestream/i_file_factory.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(IFileFactory, InstanceResolution)
{
    auto& factory = IFileFactory::instance();
    const auto file = factory.Open("/tmp/does_not_exist", std::ios_base::in | std::ios_base::out);

    EXPECT_FALSE(file.has_value());
}

}  // namespace
}  // namespace filesystem
}  // namespace score
