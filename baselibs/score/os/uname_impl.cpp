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
#include "score/os/uname_impl.h"

namespace score
{
namespace os
{

std::int32_t UnameImpl::UnameWrapper(struct utsname* info)
{
    return ::uname(info);
}

::score::cpp::optional<SystemInfo> UnameImpl::GetUname()
{
    struct utsname sysinfo{};
    std::int32_t success = UnameWrapper(&sysinfo);

    score::cpp::optional<SystemInfo> returnValue{};
    if (success != -1)
    {
        // POSIX function requires C-style array
        // NOLINTNEXTLINE(*array-to-pointer-decay, hicpp-no-array-decay) see comment above
        SystemInfo info{sysinfo.sysname, sysinfo.nodename, sysinfo.release, sysinfo.version, sysinfo.machine};
        std::ignore = returnValue.emplace(info);
    }
    return returnValue;
}

}  // namespace os
}  // namespace score
