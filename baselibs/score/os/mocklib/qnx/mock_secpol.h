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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SECPOL_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SECPOL_H

#include "score/os/qnx/secpol.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace qnx
{

class MockSecpol : public Secpol
{
  public:
    MOCK_METHOD((score::cpp::expected<secpol_file_t*, score::os::Error>),
                secpol_open,
                (const char* path, uint32_t flags),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                secpol_close,
                (secpol_file_t * handle),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                secpol_posix_spawnattr_settypeid,
                (secpol_file_t * handle, posix_spawnattr_t* attrp, const char* name, uint32_t flags),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                secpol_transition_type,
                (secpol_file_t * handle, const char* name, uint32_t flags),
                (const, noexcept, override));
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_SECPOL_H
