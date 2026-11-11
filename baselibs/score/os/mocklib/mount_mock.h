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
#ifndef SCORE_LIB_OS_MOCKLIB_MOUNT_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_MOUNT_MOCK_H

#include "score/os/mount.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

class MountMock : public Mount
{
  public:
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                mount,
                (const char*, const char*, const char*, const Mount::Flag, const void*, const std::int32_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), umount, (const char*), (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_MOUNT_MOCK_H
