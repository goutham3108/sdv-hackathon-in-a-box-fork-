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
#ifndef SCORE_LIB_OS_UNAME_IMPL_H
#define SCORE_LIB_OS_UNAME_IMPL_H

#include "score/os/uname.h"
#include <sys/utsname.h>

namespace score
{
namespace os
{

class UnameImpl : public Uname
{
  public:
    constexpr UnameImpl() = default;
    score::cpp::optional<SystemInfo> GetUname() override;

    virtual std::int32_t UnameWrapper(struct utsname* info);
};

}  // namespace os
}  // namespace score
#endif  // SCORE_LIB_OS_UNAME_IMPL_H
