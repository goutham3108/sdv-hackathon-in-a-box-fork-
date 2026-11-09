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
#ifndef SCORE_LIB_CONCURRENCY_DESTRUCTION_GUARD_H
#define SCORE_LIB_CONCURRENCY_DESTRUCTION_GUARD_H

#include <atomic>
#include <cstdint>

namespace score
{
namespace concurrency
{

/// \brief Helper to ensure that our entry counter is correctly incremented and decremented
class DestructionGuard
{
  public:
    explicit DestructionGuard(std::atomic<std::uint32_t>& counter) noexcept;
    ~DestructionGuard() noexcept;
    DestructionGuard(const DestructionGuard&) = delete;
    DestructionGuard(DestructionGuard&&) = delete;
    DestructionGuard& operator=(const DestructionGuard&) = delete;
    DestructionGuard& operator=(DestructionGuard&&) = delete;

  private:
    std::atomic<std::uint32_t>& counter_;
};

}  // namespace concurrency
}  // namespace score
#endif  // SCORE_LIB_CONCURRENCY_DESTRUCTION_GUARD_H
