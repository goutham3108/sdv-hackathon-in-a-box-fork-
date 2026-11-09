/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_ANALYSIS_TRACING_COMMON_CANARY_WRAPPER_H
#define SCORE_ANALYSIS_TRACING_COMMON_CANARY_WRAPPER_H

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

namespace score
{
namespace analysis
{
namespace tracing
{

/// @brief Template class that wraps any type with canary values for memory corruption detection
/// Memory layout: [canary_start_][data_][canary_end_]
/// @tparam T The type to protect with canaries
/// @tparam CanaryType The type of canary values (uint32_t or uint64_t)
template <typename T, typename CanaryType = std::uint64_t>
class CanaryWrapper
{
    static_assert(std::is_same_v<CanaryType, std::uint32_t> || std::is_same_v<CanaryType, std::uint64_t>,
                  "CanaryType must be uint32_t or uint64_t");

  public:
    CanaryWrapper() noexcept : canary_start_(kCanaryStart), data_{}, canary_end_(kCanaryEnd) {}

    template <typename... Args,
              typename = std::enable_if_t<std::is_constructible_v<T, Args...> && (sizeof...(Args) > 0)>>
    explicit CanaryWrapper(Args&&... args) noexcept
        : canary_start_(kCanaryStart), data_{std::forward<Args>(args)...}, canary_end_(kCanaryEnd)

    {
    }

    CanaryWrapper(const CanaryWrapper&) = default;
    CanaryWrapper& operator=(const CanaryWrapper&) = default;
    CanaryWrapper(CanaryWrapper&&) = default;
    CanaryWrapper& operator=(CanaryWrapper&&) = default;
    // Suppress "AUTOSAR C++14 M3-2-1" rule finding. This rule states:
    // "All declarations of an object or function shall have compatible types."
    // These are two distinct functions (const vs non-const member functions), not incompatible declarations
    // of the same function.
    // coverity[autosar_cpp14_m3_2_1_violation: FALSE]
    std::optional<std::reference_wrapper<const T>> GetData() const noexcept
    {
        if (IsCorrupted())
        {
            return std::nullopt;
        }
        return std::cref(data_);
    }
    // coverity[autosar_cpp14_m3_2_1_violation: FALSE]
    std::optional<std::reference_wrapper<T>> GetData() noexcept
    {
        if (IsCorrupted())
        {
            return std::nullopt;
        }
        return std::ref(data_);
    }

  private:
    static constexpr std::uint64_t kCanaryStart64 = 0xDEADBEEFCAFEBABEULL;
    static constexpr std::uint64_t kCanaryEnd64 = 0xBABECAFEBEEFDEADULL;
    static constexpr std::uint32_t kCanaryStart32 = 0xDEADBEEFU;
    static constexpr std::uint32_t kCanaryEnd32 = 0xCAFEBABEU;

    [[nodiscard]] bool IsCorrupted() const noexcept
    {
        const bool start_corrupted = (canary_start_ != kCanaryStart);
        const bool end_corrupted = (canary_end_ != kCanaryEnd);
        return start_corrupted || end_corrupted;
    }
    // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states:
    // "An integer expression shall not lead to data loss.".
    // Rationale: The cast is of the same underlying type, no data loss can occur.
    static constexpr CanaryType kCanaryStart = std::is_same_v<CanaryType, std::uint64_t>
                                                   // coverity[autosar_cpp14_a4_7_1_violation: FALSE]
                                                   ? static_cast<CanaryType>(kCanaryStart64)
                                                   : static_cast<CanaryType>(kCanaryStart32);

    static constexpr CanaryType kCanaryEnd = std::is_same_v<CanaryType, std::uint64_t>
                                                 // coverity[autosar_cpp14_a4_7_1_violation: FALSE]
                                                 ? static_cast<CanaryType>(kCanaryEnd64)
                                                 : static_cast<CanaryType>(kCanaryEnd32);

    CanaryType canary_start_;
    alignas(alignof(T)) T data_;
    CanaryType canary_end_;
};

}  // namespace tracing
}  // namespace analysis
}  // namespace score

#endif  // SCORE_ANALYSIS_TRACING_COMMON_CANARY_WRAPPER_H
