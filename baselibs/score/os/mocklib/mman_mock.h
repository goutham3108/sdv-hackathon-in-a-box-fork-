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
#ifndef SCORE_LIB_OS_MOCKLIB_MMANMOCK_H
#define SCORE_LIB_OS_MOCKLIB_MMANMOCK_H

#include "score/os/errno.h"
#include "score/os/mman.h"

#include <cstddef>
#include <cstdint>

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class MmanMock : public Mman
{
  public:
    MOCK_METHOD((score::cpp::expected<void*, Error>),
                mmap,
                (void*, std::size_t, const Mman::Protection, const Mman::Map, std::int32_t, const std::int64_t),
                (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>, munmap, (void*, const std::size_t), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                shm_open,
                (const char*, const Fcntl::Open, const Stat::Mode),
                (const, noexcept, override));
    MOCK_METHOD(score::cpp::expected_blank<Error>, shm_unlink, (const char*), (const, noexcept, override));
#if defined(__EXT_POSIX1_200112)
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                posix_typed_mem_open,
                (const char*, const Fcntl::Open, const PosixTypedMem),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                posix_typed_mem_get_info,
                (const std::int32_t, struct posix_typed_mem_info*),
                (const, noexcept, override));
#endif
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_MMANMOCK_H
