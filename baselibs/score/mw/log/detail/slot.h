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
#ifndef SCORE_MW_LOG_DETAIL_SLOT_H
#define SCORE_MW_LOG_DETAIL_SLOT_H

#include <atomic>

namespace score
{
namespace mw
{
namespace log
{

namespace detail
{

template <typename T>
class Slot
{
  public:
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init): false positive, this constructor delegates to another one
    Slot() : Slot(T{}) {};
    explicit Slot(const T& initial_value) : data_{initial_value}, in_use_{false} {}

    ~Slot() = default;
    Slot(const Slot&) noexcept = delete;
    Slot& operator=(const Slot&) noexcept = delete;
    Slot(Slot&&) noexcept = delete;
    Slot& operator=(Slot&&) noexcept = delete;

    /// Set the underlying data.
    void SetData(const T& data) noexcept
    {
        data_ = data;
    }

    /// Returns a reference to the underlying data (so it can be modified).
    T& GetDataRef() noexcept
    {
        // Suppress "AUTOSAR C++14 A9-3-1", The rule states: "Member functions shall not return non-const “raw” pointers
        // or references to private or protected data owned by the class.". This method returns a reference to the
        // internal state of the class, which is essential for allowing users to make changes directly with the variable
        // without copy
        // coverity[autosar_cpp14_a9_3_1_violation]
        return data_;
    }

    /// Return true if this slot is in use.
    bool IsUsed() const noexcept
    {
        return in_use_.load(std::memory_order_acquire);
    }

    /// Attempt to mark this slot as 'in use'. Returns true if successful,
    /// false if the slot was already in use.
    bool TryUse() noexcept
    {
        bool expected = false;
        return in_use_.compare_exchange_strong(expected, true, std::memory_order_acq_rel, std::memory_order_relaxed);
    }

    /// Mark this slot as free.
    void Release() noexcept
    {
        in_use_.store(false, std::memory_order_release);
    }

  private:
    T data_;
    std::atomic<bool> in_use_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_SLOT_H
