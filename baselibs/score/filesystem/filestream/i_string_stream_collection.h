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
#ifndef SCORE_LIB_FILESYSTEM_FILESTREAM_I_STRING_STREAM_COLLECTION_H
#define SCORE_LIB_FILESYSTEM_FILESTREAM_I_STRING_STREAM_COLLECTION_H

#include "score/filesystem/path.h"
#include "score/result/result.h"

#include <sstream>

namespace score
{
namespace filesystem
{

/// @brief The IStringStreamCollection interface combines FileFactoryFake and StandardFilesystemFake.
///
/// FileFactoryFake requires a collection that links the path and file content.
/// StandardFilesystemFake provides such a collection by implementing the IStringStreamCollection interface.
class IStringStreamCollection
{
  public:
    /// @brief Returns a reference to the string stream, if file exists; if not, creates and returns a new one.
    /// @param path The path to the file
    /// @return Reference to string stream if no error, otherwise error details.
    virtual score::Result<std::reference_wrapper<std::stringstream>> OpenStringStream(const Path& path) noexcept = 0;

    virtual ~IStringStreamCollection();
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FILESTREAM_I_STRING_STREAM_COLLECTION_H
