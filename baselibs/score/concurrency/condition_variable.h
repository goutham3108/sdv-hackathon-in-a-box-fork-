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
#ifndef SCORE_LIB_CONCURRENCY_CONDITION_VARIABLE_H
#define SCORE_LIB_CONCURRENCY_CONDITION_VARIABLE_H

#include "score/concurrency/destruction_guard.h"

#include <score/stop_token.hpp>
#include <score/utility.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <thread>

namespace score
{
namespace concurrency
{

/// \brief Extension of std::condition_variable_any with support to get woken up via score::cpp::stop_token
///
/// \details Implements parts of http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0660r10.pdf that focus on the
/// extension of std::condition_variable_any. Without this extension, the use of stop_token is rather limited.
///
/// It shall be noted that this class acts as facade towards std::condition_variable, with necessary synchronisation
/// efforts. Thus, this class comes with more memory and runtime overhead as std::condition_variable. But, if you need
/// that your condition variable gets interrupted, there is no other possibility then to use this class. All other
/// implementations will be most certainly broken.
///
/// The main point of this implementation (and what the previous implementation did wrong) is that we have to guard
/// changes within the stop token with the same mutex that we guard the conditional variable. Basically it is the same
/// story that even if you use an std::atomic as predicate for a cv, you still have to acquire the mutex beforehand.
/// This comes down to the situation, that otherwise the predicate (or stop_token) is changed while the predicate (or
/// stop_token) was evaluated. In that case we go into the wait and if no spurious wakeup is happening, we will wait for
/// ever. Since we cannot tamper the mutex that is provided by the user, we have to use our own (internal mutex).
template <typename Mutex, typename ConditionVariable>
class InterruptibleConditionalVariableBasic
{
  private:
    /// \brief Helper to ensure that given Lockable is correctly unlocked and locked (invert of std::lock_guard)
    template <typename Lockable>
    class UnlockGuard
    {
      public:
        explicit UnlockGuard(Lockable& lockable) : lockable_{lockable}
        {
            lockable_.unlock();
        }
        ~UnlockGuard()
        {
            lockable_.lock();
        }
        UnlockGuard(const UnlockGuard&) = delete;
        UnlockGuard(UnlockGuard&&) = delete;
        UnlockGuard& operator=(const UnlockGuard&) = delete;
        UnlockGuard& operator=(UnlockGuard&&) = delete;

      private:
        Lockable& lockable_;
    };

  public:
    /// \brief default constructable
    template <
        typename CV = ConditionVariable,
        typename MU = Mutex,
        std::enable_if_t<(!std::is_lvalue_reference<CV>::value) && (!std::is_lvalue_reference<MU>::value), bool> = true>
    InterruptibleConditionalVariableBasic() noexcept(std::is_nothrow_default_constructible<Mutex>::value &&
                                                     std::is_nothrow_default_constructible<ConditionVariable>::value)
        : internal_mutex_{}, internal_condition_variable_{}, entry_counter_{0u}
    {
    }

    template <typename MU = Mutex,
              typename CV = ConditionVariable,
              std::enable_if_t<std::is_lvalue_reference_v<CV> && std::is_lvalue_reference_v<MU>, bool> = true>
    explicit InterruptibleConditionalVariableBasic(Mutex mtx, ConditionVariable cv)
        : internal_mutex_{mtx}, internal_condition_variable_{cv}, entry_counter_{0u}
    {
    }

    /// \details It shall be noted that the C++ standard notes the following:
    ///
    /// Preconditions: There is no thread blocked on *this.
    /// [Note 1 : That is, all threads have been notified; they can subsequently block on the lock specified in the
    /// wait. This relaxes the usual rules, which would have required all wait calls to happen before destruction. Only
    /// the notification to unblock the wait needs to happen before destruction. Undefined behavior ensues if a thread
    /// waits on *this once the destructor has been started, especially when the waiting threads are calling the wait
    /// functions in a loop or using the overloads of wait, wait_for, or wait_until that take a predicate. — end note]
    ///
    /// The reference implementation
    /// https://github.com/josuttis/jthread/blob/master/source/condition_variable_any2.hpp#L186
    /// as also the LLVM implementation
    /// https://github.com/llvm-mirror/libcxx/blob/master/include/condition_variable#L124
    /// use both a shared_ptr to overcome this problem.
    /// The usage of a shared pointer is not acceptable in our case, thus we use entry counting. Meaning, for every
    /// entry into a wait function we increase a counter, for every exit we decrease it. Then the destructor will wait
    /// until all wait functions have been exited.
    ~InterruptibleConditionalVariableBasic()
    {
        while (entry_counter_ != 0u)
        {
            std::this_thread::yield();
        }
    }
    InterruptibleConditionalVariableBasic(const InterruptibleConditionalVariableBasic&) = delete;
    InterruptibleConditionalVariableBasic& operator=(const InterruptibleConditionalVariableBasic&) = delete;
    InterruptibleConditionalVariableBasic(InterruptibleConditionalVariableBasic&&) = delete;
    InterruptibleConditionalVariableBasic& operator=(const InterruptibleConditionalVariableBasic&&) = delete;

    /// \brief If any threads are blocked waiting for *this, unblocks one of those threads
    void notify_one() noexcept
    {
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        std::lock_guard<Mutex> guard(internal_mutex_);
        internal_condition_variable_.notify_one();
    }

    /// \brief Unblocks all threads that are blocked waiting for *this.
    void notify_all() noexcept
    {
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        std::lock_guard<Mutex> guard(internal_mutex_);
        internal_condition_variable_.notify_all();
    }

    /// \brief Wait with interrupt handling
    /// \param lock an object of type Lock that meets the BasicLockable requirements, which must be locked by the
    /// current thread
    /// \param token a score::cpp::stop_token to register interruption for
    /// \param stop_waiting predicate which returns false if the waiting should be continued (bool(stop_waiting()) ==
    /// false). \return true if stop_waiting() yields true, false otherwise (i.e. on interrupt)
    template <class Lockable, class Predicate>
    bool wait(Lockable& lock, const score::cpp::stop_token& token, Predicate stop_waiting)
    {
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        DestructionGuard guard_destruction{entry_counter_};
        if (token.stop_requested())
        {
            return stop_waiting();
        }
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        score::cpp::stop_callback callback(token, [this]() noexcept {
            notify_all();
        });
        // GCOV_EXCL_START: false-positive, covered by WaitUntilImmediatelyReturnsTrueWithPredicate
        while (!stop_waiting())
        {  // GCOV_EXCL_STOP
            // Additional context to help TSAN understand our locking and unlocking of `lock`
            {
                std::unique_lock<std::remove_reference_t<Mutex>> first_internal_lock(internal_mutex_);
                if (token.stop_requested())
                {
                    return false;
                }
                // coverity[autosar_cpp14_m0_1_9_violation] false-positive
                // coverity[autosar_cpp14_m0_1_3_violation] false-positive
                UnlockGuard<Lockable> guard_unlock(lock);
                // Move the lock to avoid false-flag from thread-sanitizer
                std::unique_lock<std::remove_reference_t<Mutex>> second_internal_lock(std::move(first_internal_lock));
                internal_condition_variable_.wait(second_internal_lock);
            }
        }

        return true;
    }

    /// \brief timed wait with interrupt handling
    /// \param lock an object of type Lock that meets the requirements of BasicLockable, which must be locked by the
    /// current thread
    /// \param token a score::cpp::stop_token to register interruption for
    /// \param abs_time an object of type std::chrono::time_point representing the time when to stop waiting
    /// \return by default true, false otherwise (i.e. on interrupt)
    template <class Lockable, class Clock, class Duration>
    std::cv_status wait_until(Lockable& lock,
                              const score::cpp::stop_token& token,
                              const std::chrono::time_point<Clock, Duration>& abs_time)
    {
        if (Clock::now() >= abs_time)
        {
            return std::cv_status::timeout;
        }

        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        DestructionGuard guard_destruction{entry_counter_};
        if (token.stop_requested())
        {
            return std::cv_status::no_timeout;
        }

        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        score::cpp::stop_callback callback(token, [this]() noexcept {
            notify_all();
        });

        const auto wait_successful = wait_until_impl(lock, token, abs_time);
        if (wait_successful)
        {
            return std::cv_status::timeout;
        }
        else
        {
            return std::cv_status::no_timeout;
        }
    }

    /// \brief timed wait with interrupt handling
    /// \param lock an object of type Lock that meets the requirements of BasicLockable, which must be locked by the
    /// current thread
    /// \param token a score::cpp::stop_token to register interruption for
    /// \param abs_time an object of type std::chrono::time_point representing the time when to stop waiting
    /// \param stop_waiting predicate which returns false if the waiting should be continued (bool(stop_waiting()) ==
    /// false) \return true if stop_waiting() yields true, false otherwise (i.e. on timeout or interrupt)
    template <class Lockable, class Clock, class Duration, class Predicate>
    bool wait_until(Lockable& lock,
                    const score::cpp::stop_token& token,
                    const std::chrono::time_point<Clock, Duration>& abs_time,
                    Predicate stop_waiting)
    {
        if (abs_time == std::chrono::time_point<Clock, Duration>::max())
        {
            // Suppress "AUTOSAR C++14 A5-1-4" rule finding: "A lambda expression object shall not outlive any of its
            // reference-captured objects.".
            // Predicate "stop_waiting" is passed by move-semantic to wait_until(), all dangling references that
            // this predicate can capture is the awareness and responsibility of a caller
            // coverity[autosar_cpp14_a5_1_4_violation]
            return wait(lock, token, std::move(stop_waiting));
        }
        if (Clock::now() >= abs_time)
        {
            return stop_waiting();
        }

        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        DestructionGuard guard_destruction{entry_counter_};
        if (token.stop_requested())
        {
            return stop_waiting();
        }

        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        score::cpp::stop_callback callback(token, [this]() noexcept {
            notify_all();
        });  // GCOV_EXCL_START: false-positive, covered by WaitUntilImmediatelyReturnsTrueWithPredicate
        while (!stop_waiting())
        // GCOV_EXCL_STOP
        {
            bool shouldStop{false};
            {
                score::cpp::ignore = wait_until_impl(lock, token, abs_time);

                // Explicitly check that `abs_time` was reached, to avoid bug in standard library on POSIX systems.
                // std::condition_variable::wait_until calls pthread_cond_timedwait on POSIX systems.
                // pthread_cond_timedwait uses the monotonic clock from the system to wait the relative amount of time
                // equivalent to the distance between `abs_time` and `Clock::now()`. Should `Clock` not be monotonic,
                // this might lead to spurious wakeups before `abs_time` was actually reached. The C++ standard says
                // that wait_until shall return std::cv_status::no_timeout in such cases. This is not true for POSIX
                // systems, where the return value is determined based on the monotonic clock of the system and not
                // `Clock`. See also [thread.req.timing] in the C++ standard.
                shouldStop = (Clock::now() >= abs_time) || token.stop_requested();
            }
            if (shouldStop)
            {
                return stop_waiting();
            }
        }
        return true;
    }

    /// \brief timed wait with interrupt handling
    /// \param lock an object of type Lock that meets the BasicLockable requirements, which must be locked by the
    /// current thread
    /// \param token a score::cpp::stop_token to register interruption for
    /// \param rel_time an object of type std::chrono::duration representing the maximum time to spend waiting.
    /// Note that rel_time must be small enough not to overflow when added to std::chrono::steady_clock::now().
    /// \param stop_waiting predicate which returns false if the waiting should be continued (bool(stop_waiting()) ==
    /// false).
    /// \return true if stop_waiting() yields true, false otherwise (i.e. on timeout or interrupt)
    template <class Lockable, class Rep, class Period, class Predicate>
    bool wait_for(Lockable& lock,
                  const score::cpp::stop_token& token,
                  const std::chrono::duration<Rep, Period>& rel_time,
                  Predicate stop_waiting)
    {
        const auto abs_time = std::chrono::steady_clock::now() + rel_time;
        // Suppress "AUTOSAR C++14 A5-1-4" rule finding: "A lambda expression object shall not outlive any of its
        // reference-captured objects.".
        // Predicate "stop_waiting" is passed by move-semantic to wait_until(), all dangling references that
        // this predicate can capture is the awareness and responsibility of a caller
        // coverity[autosar_cpp14_a5_1_4_violation]
        return wait_until(lock, token, abs_time, std::move(stop_waiting));
    }

  private:
    /// \brief timed wait with interrupt handling
    /// \param lock an object of type Lock that meets the requirements of BasicLockable, which must be locked by the
    /// current thread
    /// \param token a score::cpp::stop_token to register interruption for
    /// \param abs_time an object of type std::chrono::time_point representing the time when to stop waiting
    /// \return by default true, false otherwise (i.e. on interrupt)
    template <class Lockable, class Clock, class Duration>
    bool wait_until_impl(Lockable& lock,
                         const score::cpp::stop_token& token,
                         const std::chrono::time_point<Clock, Duration>& abs_time)
    {
        std::unique_lock<std::remove_reference_t<Mutex>> first_internal_lock(internal_mutex_);
        // GCOV_EXCL_START: false-positive, covered by WaitUntilImmediatelyReturnIfStopIsRequesteAfterLocking
        if (token.stop_requested())
        // GCOV_EXCL_STOP
        {
            return false;
        }
        // coverity[autosar_cpp14_m0_1_3_violation] false-positive
        // coverity[autosar_cpp14_m0_1_9_violation] false-positive
        UnlockGuard<Lockable> guard_unlock(lock);
        // Move the lock to avoid false-flag from thread-sanitizer
        std::unique_lock<std::remove_reference_t<Mutex>> second_internal_lock(std::move(first_internal_lock));
        score::cpp::ignore = internal_condition_variable_.wait_until(second_internal_lock, abs_time);

        return not token.stop_requested();
    }

    Mutex internal_mutex_;
    ConditionVariable internal_condition_variable_;
    std::atomic<std::uint32_t> entry_counter_;
};

using InterruptibleConditionalVariable = InterruptibleConditionalVariableBasic<std::mutex, std::condition_variable>;

}  // namespace concurrency
}  // namespace score
#endif  // SCORE_LIB_CONCURRENCY_CONDITION_VARIABLE_H
