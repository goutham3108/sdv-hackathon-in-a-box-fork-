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
#ifndef SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_PROMISE_H
#define SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_PROMISE_H

#include "score/concurrency/future/base_interruptible_promise.h"
#include "score/concurrency/future/error.h"

#include "score/expected.hpp"

namespace score
{
namespace concurrency
{

/**
 * Interruptible promise used to provide state to InterruptibleFuture or InterruptibleSharedFuture. Close
 * reimplementation of std::promise.
 *
 * \tparam Value The type of the values stored in the shared state
 */
template <typename Value>
class InterruptiblePromise final : public detail::BaseInterruptiblePromise<Value>
{
  public:
    /**
     * Stores the value in the shared state and makes the state ready.
     *
     * Calls to this function do NOT introduce data races with calls to GetInterruptibleFuture() or SetError().
     *
     * \param value Value to store in the shared state
     * \returns Blank expected on success or with Error if:
     * - no shared state is associated
     * - shared state already stores a value or error
     */
    score::cpp::expected_blank<Error> SetValue(const Value& value) noexcept
    {
        constexpr auto strategy = [](InterruptibleState<Value>& state, const Value& val) noexcept {
            return state.SetValue(val);
        };
        // Suppress "AUTOSAR C++14 A5-1-4" rule finding: "A lambda expression object shall not outlive any of its
        // reference-captured objects.".
        // If lambda "strategy" captures referenced objects, the lambda will not outlive those references. In fact, in
        // the current implementation, no dangling references can occur, since SetValueInternalWithStrategy() currently
        // does not keep a copy of the lambda.
        // coverity[autosar_cpp14_a5_1_4_violation]
        return this->SetValueInternalWithStrategy(strategy, value);
    }

    /**
     * Stores the value in the shared state and makes the state ready.
     *
     * Calls to this function do NOT introduce data races with calls to GetInterruptibleFuture() or SetError().
     *
     * \param value Value to store in the shared state
     * \returns Blank expected on success or with Error if:
     * - no shared state is associated
     * - shared state already stores a value or error
     */
    score::cpp::expected_blank<Error> SetValue(Value&& value) noexcept
    {
        constexpr auto strategy = [](InterruptibleState<Value>& state, Value&& val) noexcept {
            return state.SetValue(std::move(val));
        };
        // Suppress "AUTOSAR C++14 A5-1-4" rule finding: "A lambda expression object shall not outlive any of its
        // reference-captured objects.".
        // If lambda "strategy" captures referenced objects, the lambda will not outlive those references. In fact, in
        // the current implementation, no dangling references can occur, since SetValueInternalWithStrategy() currently
        // does not keep a copy of the lambda.
        // coverity[autosar_cpp14_a5_1_4_violation]
        return this->SetValueInternalWithStrategy(strategy, std::move(value));
    }
};

/**
 * Interruptible promise used to provide state to InterruptibleFuture or InterruptibleSharedFuture. Close
 * reimplementation of std::promise.
 *
 * Specialization for reference-types.
 *
 * \tparam Value The type of the values stored in the shared state
 */
template <typename Value>
class InterruptiblePromise<Value&> final : public detail::BaseInterruptiblePromise<Value&>
{
  public:
    /**
     * Stores the value in the shared state and makes the state ready.
     *
     * Calls to this function do NOT introduce data races with calls to GetInterruptibleFuture() or SetError().
     *
     * \param value Value to store in the shared state
     * \returns Blank expected on success or with Error if:
     * - no shared state is associated
     * - shared state already stores a value or error
     */
    score::cpp::expected_blank<Error> SetValue(Value& value) noexcept
    {
        constexpr auto strategy = [](InterruptibleState<Value&>& state, Value& val) noexcept {
            return state.SetValue(val);
        };
        return this->SetValueInternalWithStrategy(strategy, value);
    }
};

/**
 * Interruptible promise used to provide state to InterruptibleFuture or InterruptibleSharedFuture. Close
 * reimplementation of std::promise.
 *
 * Specialization for void.
 */
template <>
class InterruptiblePromise<void> final : public detail::BaseInterruptiblePromise<void>
{
  public:
    /**
     * Stores the value in the shared state and makes the state ready.
     *
     * Calls to this function do NOT introduce data races with calls to GetInterruptibleFuture() or SetError().
     *
     * \returns Blank expected on success or with Error if:
     * - no shared state is associated
     * - shared state already stores a value or error
     */
    score::cpp::expected_blank<Error> SetValue() noexcept;
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_PROMISE_H
