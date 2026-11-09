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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_INSTRUMENTED_MEMORY_RESOURCE_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_INSTRUMENTED_MEMORY_RESOURCE_H

#include <score/memory_resource.hpp>

#include <atomic>
#include <cstdint>

namespace score::safecpp
{

class InstrumentedMemoryResourceInstanceIdProvider
{
  public:
    constexpr InstrumentedMemoryResourceInstanceIdProvider() noexcept = default;
    ~InstrumentedMemoryResourceInstanceIdProvider() noexcept = default;
    InstrumentedMemoryResourceInstanceIdProvider(const InstrumentedMemoryResourceInstanceIdProvider&) = delete;
    InstrumentedMemoryResourceInstanceIdProvider& operator=(const InstrumentedMemoryResourceInstanceIdProvider&) =
        delete;
    InstrumentedMemoryResourceInstanceIdProvider(InstrumentedMemoryResourceInstanceIdProvider&&) noexcept = delete;
    InstrumentedMemoryResourceInstanceIdProvider& operator=(InstrumentedMemoryResourceInstanceIdProvider&&) noexcept =
        delete;

    [[nodiscard]] static std::size_t GetNextFreeInstanceId() noexcept
    {
        static InstrumentedMemoryResourceInstanceIdProvider instance_id_provider{};
        return instance_id_provider.id_++;
    }

  private:
    std::atomic<std::size_t> id_{0U};
};

class InstrumentedMemoryResource : public score::cpp::pmr::memory_resource
{
  public:
    [[nodiscard]] std::int32_t GetNumberOfAllocations() const noexcept;

  private:
    std::size_t id_{InstrumentedMemoryResourceInstanceIdProvider::GetNextFreeInstanceId()};
    std::int32_t active_allocations_{0};
    score::cpp::pmr::memory_resource* memory_resource_{score::cpp::pmr::new_delete_resource()};

    [[nodiscard]] void* do_allocate(std::size_t bytes, std::size_t alignment) override;

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override;

    [[nodiscard]] bool do_is_equal(const memory_resource&) const noexcept override;
};

}  // namespace score::safecpp

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_TESTING_INSTRUMENTED_MEMORY_RESOURCE_H
