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
#ifndef SCORE_LIB_OS_UTILS_THREAD_H
#define SCORE_LIB_OS_UTILS_THREAD_H

#include <score/jthread.hpp>
#include <cstddef>
#include <string>
#include <thread>

namespace score
{
namespace os
{

/// @brief set thread affinity to a single cpu for the calling thread
///
/// @param cpu Cpu id starting from zero
/// @return true on success
bool set_thread_affinity(const std::size_t cpu) noexcept;

void set_thread_name(const pthread_t& thread, const std::string& name) noexcept;
void set_thread_name(std::thread& thread, const std::string& name) noexcept;
void set_thread_name(score::cpp::jthread& thread, const std::string& name) noexcept;

std::string get_thread_name(std::thread& thread);

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_THREAD_H
