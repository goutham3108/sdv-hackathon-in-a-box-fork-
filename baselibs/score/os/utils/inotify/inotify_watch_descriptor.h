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
#ifndef SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_WATCH_DESCRIPTOR_H
#define SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_WATCH_DESCRIPTOR_H

#include <cstdint>

// For std::hash
#include <optional>

namespace score
{
namespace os
{

class InotifyWatchDescriptor
{
  public:
    explicit InotifyWatchDescriptor(std::int32_t watch_descriptor) noexcept;

    std::int32_t GetUnderlying() const noexcept;

  private:
    std::int32_t watch_descriptor_{-1};
};

bool operator==(const InotifyWatchDescriptor& lhs, const InotifyWatchDescriptor& rhs) noexcept;

bool operator!=(const InotifyWatchDescriptor& lhs, const InotifyWatchDescriptor& rhs) noexcept;

}  // namespace os
}  // namespace score

namespace std
{

template <>
class hash<score::os::InotifyWatchDescriptor>
{
  public:
    std::size_t operator()(const score::os::InotifyWatchDescriptor& watch_descriptor) const noexcept;
};

}  // namespace std

#endif  // SCORE_LIB_OS_UTILS_INOTIFY_INOTIFY_WATCH_DESCRIPTOR_H
