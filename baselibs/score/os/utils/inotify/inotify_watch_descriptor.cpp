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
#include "score/os/utils/inotify/inotify_watch_descriptor.h"

namespace score
{
namespace os
{

InotifyWatchDescriptor::InotifyWatchDescriptor(std::int32_t watch_descriptor) noexcept
    : watch_descriptor_{watch_descriptor}
{
}

std::int32_t InotifyWatchDescriptor::GetUnderlying() const noexcept
{
    return watch_descriptor_;
}

bool operator==(const InotifyWatchDescriptor& lhs, const InotifyWatchDescriptor& rhs) noexcept
{
    return lhs.GetUnderlying() == rhs.GetUnderlying();
}

bool operator!=(const InotifyWatchDescriptor& lhs, const InotifyWatchDescriptor& rhs) noexcept
{
    return lhs.GetUnderlying() != rhs.GetUnderlying();
}

}  // namespace os
}  // namespace score

namespace std
{

std::size_t std::hash<score::os::InotifyWatchDescriptor>::operator()(
    const score::os::InotifyWatchDescriptor& watch_descriptor) const noexcept
{
    return std::hash<std::int32_t>()(watch_descriptor.GetUnderlying());
}

}  // namespace std
