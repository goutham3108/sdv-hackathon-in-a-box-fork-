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
#ifndef SCORE_LIB_OS_UTILS_INTERPROCESSMUTEX_H
#define SCORE_LIB_OS_UTILS_INTERPROCESSMUTEX_H

#include <sys/types.h>

namespace score
{
namespace os
{

// Suppress "AUTOSAR C++14 M3-2-3" rule finding: "A type, object or function that is used in multiple translation units
// shall be declared in one and only one file.".
// Rationale: This is false positive because this is just the forward declaration of existing class to
// avoid circular dependecy issue.
// coverity[autosar_cpp14_m3_2_3_violation]
class InterprocessConditionalVariable;

/**
 * \brief InterprocessMutex in difference to std::mutex can be used to synchronize multiple processes (e.g. over shared
 * memory) \details Implements C++ BasicLockable and Lockable requirements
 */
// Suppress "AUTOSAR C++14 M3-2-3" rule finding: "A type, object or function that is used in multiple translation units
// shall be declared in one and only one file.".
// Rationale: This is false positive because file header include guards ensures ODR.
// coverity[autosar_cpp14_m3_2_3_violation]
class InterprocessMutex
{
  public:
    explicit InterprocessMutex() noexcept;
    InterprocessMutex(const InterprocessMutex&) = delete;
    InterprocessMutex& operator=(const InterprocessMutex) = delete;
    InterprocessMutex(InterprocessMutex&&) noexcept = delete;
    InterprocessMutex& operator=(InterprocessMutex&&) noexcept = delete;
    ~InterprocessMutex() noexcept;

    /**
     * \brief Blocks until a lock can be obtained for the current execution agent (thread, process, task).
     *        If an exception is thrown, no lock is obtained.
     */
    void lock() noexcept;

    /**
     * \brief Releases the lock held by the execution agent. Throws no exceptions.
     */
    void unlock() noexcept;

    /**
     * \brief Attempts to acquire the lock for the current execution agent (thread, process, task) without blocking. If
     * an exception is thrown, no lock is obtained.
     * @return true if the lock was acquired, false otherwise
     */
    bool try_lock() noexcept;

  private:
    pthread_mutex_t mutex{};
    // Suppress "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Rationale: Friend is used as the class InterprocessConditionalVariable needs access to private &
    // protected members of InterprocessMutex and both classes are tightly coupled.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend InterprocessConditionalVariable;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_INTERPROCESSMUTEX_H
