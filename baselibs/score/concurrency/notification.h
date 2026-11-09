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
#ifndef SCORE_LIB_CONCURRENCY_NOTIFICATION_H
#define SCORE_LIB_CONCURRENCY_NOTIFICATION_H

#include "score/concurrency/destruction_guard.h"
#include "score/concurrency/interruptible_wait.h"

#include "score/stop_token.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

namespace score
{
namespace concurrency
{

/**
 * \brief The Notification allows to receive
 * notifications synchronized by different processes.
 *
 * The main idea is that a thread is waiting
 * for a specific notification to happen.
 * Another thread could then `notify()` all waiting
 * threads for this notification.
 *
 * It shall be noted that this is a one-shot solution.
 * After a notification was received threads will not wait until
 * `notify()` is invoked again, unless reset() was called before.
 *
 * As an conditional variable, this class is neither copyable nor moveable.
 *
 * \detail Overall this is only small abstraction around a conditional
 * variable that protects a boolean (if notified was invoked).
 *
 * For a version that supports interprocess synchronization please checkout score::os::InterprocessNotification
 */
template <typename Mutex, typename ConditionVariable>
class NotificationBasic
{
  public:
    /**
     * \brief Constructs Notification concluding that no
     * notification has happened yet
     *
     * \post first call to waitWithAbort() will block
     */
    template <
        typename CV = ConditionVariable,
        typename MU = Mutex,
        std::enable_if_t<(!std::is_lvalue_reference<CV>::value) && (!std::is_lvalue_reference<MU>::value), bool> = true>
    NotificationBasic() noexcept(std::is_nothrow_default_constructible<Mutex>::value &&
                                 std::is_nothrow_default_constructible<ConditionVariable>::value)
        : mutex_{}, conditional_variable_{}, notified_yet_{false}, entry_counter_{0u}
    {
    }

    template <typename CV = ConditionVariable, std::enable_if_t<std::is_lvalue_reference<CV>::value, bool> = true>
    explicit NotificationBasic(ConditionVariable cv)
        : mutex_{}, conditional_variable_{cv}, notified_yet_{false}, entry_counter_{0u}
    {
    }

    NotificationBasic(const NotificationBasic&) = delete;
    NotificationBasic(NotificationBasic&&) = delete;
    NotificationBasic& operator=(const NotificationBasic&) = delete;
    NotificationBasic& operator=(NotificationBasic&&) = delete;
    ~NotificationBasic()
    {
        while (entry_counter_ != 0u)
        {
            std::this_thread::yield();
        }
    }

    /**
     * \brief Blocks execution until either:
     * - notify() is invoked
     * - stop_token.stop_requested() == true
     *
     * No spurious wake-ups can happen.
     *
     * \param token A stop_token that can abort any wait
     *
     * \return true if a notification was received, false otherwise
     */
    bool waitWithAbort(score::cpp::stop_token token)
    {
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        DestructionGuard guard{entry_counter_};
        std::unique_lock<Mutex> lock{mutex_};
        return conditional_variable_.wait(lock, token, [this]() noexcept {
            return notified_yet_;
        });
    }

    /**
     * \brief Blocks execution until either:
     * - notify() is invoked
     * - stop_token.stop_requested() == true
     * - time passed by
     *
     * No spurious wake-ups can happen
     *
     * \tparam Rep an arithmetic type representing the number of ticks (from std::chrono)
     * \tparam Period a std::ratio representing the tick period (i.e. the number of seconds per tick) (from std::chrono)
     *
     * \param time The time that shall be waited
     * \param token A stop_token that can abort any wait
     * \return true if a notification was received, false otherwise
     */
    template <class Rep, class Period>
    inline bool waitForWithAbort(const std::chrono::duration<Rep, Period>& duration, score::cpp::stop_token token)
    {
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        DestructionGuard guard{entry_counter_};
        std::unique_lock<Mutex> lock{mutex_};
        return conditional_variable_.wait_for(lock, token, duration, [this]() noexcept {
            return notified_yet_;
        });
    }

    /**
     * \brief Notifies all waiting threads
     *
     * \post All threads waiting on waitWithAbort() will continue
     */
    void notify()
    {
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        DestructionGuard guard{entry_counter_};
        {
            {
                // coverity[autosar_cpp14_m0_1_9_violation] false-positive
                // coverity[autosar_cpp14_m0_1_3_violation] false-positive
                std::lock_guard<Mutex> lock{mutex_};
                notified_yet_ = true;
            }
            conditional_variable_.notify_all();
        }
    }

    /**
     * \brief Resets notification
     *
     * \post All call to waitForWithAbort() will block again, until notify() was invoked
     */
    void reset()
    {
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        DestructionGuard guard{entry_counter_};
        {
            // coverity[autosar_cpp14_m0_1_9_violation] false-positive
            // coverity[autosar_cpp14_m0_1_3_violation] false-positive
            std::lock_guard<Mutex> lock{mutex_};
            notified_yet_ = false;
        }
    }

  private:
    Mutex mutex_;
    ConditionVariable conditional_variable_;
    bool notified_yet_;
    std::atomic<std::uint32_t> entry_counter_;
};

using Notification = NotificationBasic<std::mutex, InterruptibleConditionalVariable>;

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_NOTIFICATION_H
