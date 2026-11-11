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
#include "score/os/utils/inotify/inotify_event.h"

namespace score
{
namespace os
{

InotifyEvent::InotifyEvent(const struct ::inotify_event& event)
    : watch_descriptor_{event.wd}, mask_{IntegerToReadMask(event.mask)}, cookie_{event.cookie}, name_{}
{
    if (event.len > 0U)
    {
        // Event.name is a null-terminated string stored in a flexible array, and string_view can safely reference it.
        // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay) see comment above
        std::string_view name_view{event.name};
        name_ = score::cpp::static_vector<char, NAME_MAX + 1>{name_view.begin(), name_view.end()};
        name_.push_back('\0');
    }
}

InotifyWatchDescriptor InotifyEvent::GetWatchDescriptor() const noexcept
{
    return watch_descriptor_;
}

InotifyEvent::ReadMask InotifyEvent::GetMask() const noexcept
{
    return mask_;
}

std::uint32_t InotifyEvent::GetCookie() const noexcept
{
    return cookie_;
}

std::string_view InotifyEvent::GetName() const noexcept
{
    return std::string_view{name_.data()};
}

InotifyEvent::ReadMask InotifyEvent::IntegerToReadMask(const std::uint32_t native_event_mask) noexcept
{
    ReadMask event_mask{};
    if ((native_event_mask & static_cast<std::uint32_t>(IN_ACCESS)) != 0U)
    {
        event_mask |= ReadMask::kInAccess;
    }
    if ((native_event_mask & static_cast<std::uint32_t>(IN_MOVED_TO)) != 0U)
    {
        event_mask |= ReadMask::kInMovedTo;
    }
    if ((native_event_mask & static_cast<std::uint32_t>(IN_CREATE)) != 0U)
    {
        event_mask |= ReadMask::kInCreate;
    }
    if ((native_event_mask & static_cast<std::uint32_t>(IN_DELETE)) != 0U)
    {
        event_mask |= ReadMask::kInDelete;
    }
    if ((native_event_mask & static_cast<std::uint32_t>(IN_IGNORED)) != 0U)
    {
        event_mask |= ReadMask::kInIgnored;
    }
    if ((native_event_mask & static_cast<std::uint32_t>(IN_ISDIR)) != 0U)
    {
        event_mask |= ReadMask::kInIsDir;
    }
    if ((native_event_mask & static_cast<std::uint32_t>(IN_Q_OVERFLOW)) != 0U)
    {
        event_mask |= ReadMask::kInQOverflow;
    }
    return event_mask;
}

bool operator==(const InotifyEvent& lhs, const InotifyEvent& rhs) noexcept
{
    return ((lhs.GetWatchDescriptor() == rhs.GetWatchDescriptor()) && (lhs.GetMask() == rhs.GetMask())) &&
           ((lhs.GetCookie() == rhs.GetCookie()) && (lhs.GetName() == rhs.GetName()));
}

}  // namespace os
}  // namespace score
