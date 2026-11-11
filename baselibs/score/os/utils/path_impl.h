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
#ifndef SCORE_OS_UTILS_PATH_IMPL_H
#define SCORE_OS_UTILS_PATH_IMPL_H

#include "score/os/utils/path.h"

namespace score
{
namespace os
{

class PathImpl final : public Path
{
  public:
    constexpr PathImpl() = default;
    std::string get_base_name(const std::string& path) const noexcept override;

    /// @brief Returns the parent directory name of `path`.
    std::string get_parent_dir(const std::string& path) const noexcept override;

    /// @brief Returns the absolute path to the executable
    score::cpp::expected<std::string, score::os::Error> get_exec_path() const noexcept override;
};

}  // namespace os
}  // namespace score

#endif
