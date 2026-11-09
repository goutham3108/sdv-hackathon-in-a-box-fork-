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
#include "score/filesystem/factory/filesystem_factory_fake.h"

namespace score
{
namespace filesystem
{

using ::testing::Invoke;

FilesystemFactoryFake::FilesystemFactoryFake()
{
    score::cpp::ignore =
        ON_CALL(*this, CreateInstance()).WillByDefault(Invoke(this, &FilesystemFactoryFake::FakeCreateInstance));
}

Filesystem FilesystemFactoryFake::FakeCreateInstance() const noexcept
{
    return {standard_, utils_, streams_};
}

StandardFilesystemFake& FilesystemFactoryFake::GetStandard() const noexcept
{
    return *standard_;
}
FileUtilsFake& FilesystemFactoryFake::GetUtils() const noexcept
{
    return *utils_;
}
FileFactoryFake& FilesystemFactoryFake::GetStreams() const noexcept
{
    return *streams_;
}

}  // namespace filesystem
}  // namespace score
