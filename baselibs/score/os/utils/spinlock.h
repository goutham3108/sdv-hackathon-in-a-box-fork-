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
#ifndef SCORE_LIB_OS_UTILS_SPINLOCK_H
#define SCORE_LIB_OS_UTILS_SPINLOCK_H

#include <atomic>

namespace score
{
namespace os
{

/**
 * @brief Implementation of a spinlock based on std::atomic
 *
 * This implementation is based on insights regarding spinlock performance depending on usage of correct memory orders
 * and tuned for good behaviour on multicore architectures with cache coherency protocols.
 *
 * This class fulfills the requirements of <tt>Lockable</tt> (@see https://en.cppreference.com/w/cpp/named_req/Lockable)
 *
 * Implementation is based on/derived from ideas here: @see https://rigtorp.se/spinlock/
 */
class Spinlock
{
  public:
    Spinlock() noexcept;

    ~Spinlock() noexcept;

    /**
     * Spinlock is not copyable (as underlying std::atomic isn't copyable)
     */
    Spinlock(const Spinlock& other) = delete;
    /**
     * Spinlock is not movable (as underlying std::atomic isn't movable)
     */
    Spinlock(Spinlock&& other) = delete;

    /**
     * Spinlock isn't copyable and therefore not copy assignable.
     */
    Spinlock& operator=(const Spinlock& other) = delete;
    /**
     * Spinlock isn't movable and therefore not move assignable.
     */
    Spinlock& operator=(const Spinlock&& other) = delete;

    /**
     * @brief try to acquire the lock. If it fails, return immediately.
     * @return  <tt>true</tt> in case the lock could be acquired, <tt>false</tt> else.
     */
    bool try_lock() noexcept;

    /**
     * @brief Blocking lock acquire. Returns is lock is held.
     *
     * Implementation tries first to acquire the lock with <tt>exchange</tt>. If this fails, it "spins" on a purely
     * lock read semantics with <tt>load()</tt> as this means less cache coherency traffic. Only if the lock seems free
     * again it goes back to really acquire the lock with <tt>exchange</tt> again.
     *
     * @note In the referenced original proposal a mechanism to use a more resource consumption friendly "spinning" was
     * mentioned, by using a processor architecture specific operation in the spinning loop.
     * In case of the referenced proposal it was an x86 architecture specific functionality (__builtin_ia32_pause()).
     * But since we want to be arch agnostic here, we leave that out.
     */
    void lock() noexcept;

    /**
     * @brief unlock the held lock.
     */
    void unlock() noexcept;

  private:
    std::atomic<bool> atomic_lock;
};
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_SPINLOCK_H
