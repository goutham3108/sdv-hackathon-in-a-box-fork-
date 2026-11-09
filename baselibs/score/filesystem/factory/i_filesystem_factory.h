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
#ifndef SCORE_LIB_FILESYSTEM_FACTORY_I_FILESYSTEM_FACTORY_H
#define SCORE_LIB_FILESYSTEM_FACTORY_I_FILESYSTEM_FACTORY_H

#include "score/filesystem/filesystem_struct.h"

namespace score
{
namespace filesystem
{

class IFilesystemFactory
{
  public:
    IFilesystemFactory() = default;
    virtual ~IFilesystemFactory() noexcept;

    IFilesystemFactory(const IFilesystemFactory&) = delete;
    IFilesystemFactory(const IFilesystemFactory&&) noexcept = delete;
    IFilesystemFactory& operator=(const IFilesystemFactory&) = delete;
    IFilesystemFactory& operator=(const IFilesystemFactory&&) noexcept = delete;

    /// \brief Returns instance of Filesystem structure with smart pointers to filesystem library interfaces.
    virtual Filesystem CreateInstance() const noexcept = 0;
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FACTORY_I_FILESYSTEM_FACTORY_H
