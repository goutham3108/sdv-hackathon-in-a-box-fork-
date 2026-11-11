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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_TESTING_ALLOCATOR_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_TESTING_ALLOCATOR_H

#include <atomic>
#include <cstdint>
#include <memory>

namespace score::safecpp::details
{

class InstanceIdProvider
{
  public:
    constexpr InstanceIdProvider() noexcept = default;
    ~InstanceIdProvider() noexcept = default;
    InstanceIdProvider(const InstanceIdProvider&) = delete;
    InstanceIdProvider& operator=(const InstanceIdProvider&) = delete;
    InstanceIdProvider(InstanceIdProvider&&) noexcept = delete;
    InstanceIdProvider& operator=(InstanceIdProvider&&) noexcept = delete;

    [[nodiscard]] static std::size_t GetNextFreeInstanceId() noexcept
    {
        static InstanceIdProvider instance_id_provider{};
        return instance_id_provider.id_++;
    }

  private:
    std::atomic<std::size_t> id_{0U};
};

template <class T, class Propagating>
class TestingAllocator
{
  public:
    using value_type = T;
    using propagate_on_container_copy_assignment = Propagating;
    using propagate_on_container_move_assignment = Propagating;
    using propagate_on_container_swap = Propagating;

    TestingAllocator() noexcept : allocator_{}, id_{InstanceIdProvider::GetNextFreeInstanceId()} {}

    template <class U>
    // NOLINTNEXTLINE(*-explicit-constructor) Implicit conversion required by C++ standard for rebinding
    TestingAllocator(const TestingAllocator<U, Propagating>& other) : allocator_{}, id_{other.id_}
    {
    }

    [[nodiscard]] T* allocate(std::size_t n)
    {
        return allocator_.allocate(n);
    }

    void deallocate(T* p, std::size_t n)
    {
        allocator_.deallocate(p, n);
    }

    template <class U, class OtherPropagating>
    [[nodiscard]] bool operator==(const TestingAllocator<U, OtherPropagating>& other) const
    {
        return id_ == other.id_;
    }

    template <class U, class OtherPropagating>
    [[nodiscard]] bool operator!=(const TestingAllocator<U, OtherPropagating>& other) const
    {
        return id_ != other.id_;
    }

  private:
    std::allocator<T> allocator_;
    std::size_t id_;

    template <class A, class B>
    friend class TestingAllocator;
};

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_TESTING_ALLOCATOR_H
