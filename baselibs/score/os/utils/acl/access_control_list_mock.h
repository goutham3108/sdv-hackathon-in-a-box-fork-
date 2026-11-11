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
#ifndef SCORE_LIB_OS_UTILS_ACL_ACCESS_CONTROL_LIST_MOCK_H
#define SCORE_LIB_OS_UTILS_ACL_ACCESS_CONTROL_LIST_MOCK_H

#include "score/os/utils/acl/i_access_control_list.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class AccessControlListMock final : public IAccessControlList
{
  public:
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                AllowUser,
                (const UserIdentifier, const Acl::Permission),
                (override));

    MOCK_METHOD((score::cpp::expected<bool, score::os::Error>),
                VerifyMaskPermissions,
                (const std::vector<::score::os::Acl::Permission>&),
                (const, override));

    MOCK_METHOD((score::cpp::expected<std::vector<UserIdentifier>, score::os::Error>),
                FindUserIdsWithPermission,
                (const Acl::Permission),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_ACL_ACCESS_CONTROL_LIST_MOCK_H
