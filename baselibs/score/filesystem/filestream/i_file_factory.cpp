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
#include "score/filesystem/filestream/i_file_factory.h"

#include "score/filesystem/filestream/file_factory.h"

namespace score
{
namespace filesystem
{

IFileFactory& IFileFactory::instance() noexcept
{
    // coverity[autosar_cpp14_a3_3_2_violation] singleton
    static FileFactory instance{};  // LCOV_EXCL_BR_LINE caused by exceptions
    return select_instance(instance);
}

IFileFactory::~IFileFactory() noexcept = default;

IFileFactory::IFileFactory() noexcept = default;

// Different LCOV behavior: covered on Linux but not on QNX8, excluded from coverage for QNX8
IFileFactory::IFileFactory(const IFileFactory&) noexcept = default;  // LCOV_EXCL_LINE

IFileFactory& IFileFactory::operator=(const IFileFactory&) & noexcept = default;  // LCOV_EXCL_LINE
IFileFactory::IFileFactory(IFileFactory&&) noexcept = default;                    // LCOV_EXCL_LINE
IFileFactory& IFileFactory::operator=(IFileFactory&&) & noexcept = default;       // LCOV_EXCL_LINE

}  // namespace filesystem
}  // namespace score
