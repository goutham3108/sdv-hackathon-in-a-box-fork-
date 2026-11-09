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
#include "score/filesystem/filestream/simple_string_stream_collection.h"

#include "score/filesystem/path.h"
#include "score/result/result.h"

#include <sstream>

namespace score
{
namespace filesystem
{

score::Result<std::reference_wrapper<std::stringstream>> SimpleStringStreamCollection::OpenStringStream(
    const Path& path) noexcept
{
    return files_[path];
}

}  // namespace filesystem
}  // namespace score
