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
#include "score/filesystem/filestream/bad_string_stream_collection.h"

#include "score/filesystem/error.h"

namespace score
{
namespace filesystem
{

Result<std::reference_wrapper<std::stringstream>> BadStringStreamCollection::OpenStringStream(const Path&) noexcept
{
    return MakeUnexpected(ErrorCode::kCouldNotOpenFileStream);
}

}  // namespace filesystem
}  // namespace score
