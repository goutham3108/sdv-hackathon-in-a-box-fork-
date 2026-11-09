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
#include "score/os/utils/mocklib/threadmock.h"
#include <thread>

namespace score
{
namespace os
{

namespace
{
std::function<bool(const std::size_t)> set_thread_affinity_cb;
std::function<void(std::thread&, const std::string&)> set_thread_name_cb;
std::function<std::string(std::thread&)> get_thread_name_cb;
}  // namespace

ThreadMock::ThreadMock()
{
    set_thread_affinity_cb = [this](const std::size_t cpu) {
        return this->set_thread_affinity(cpu);
    };
    set_thread_name_cb = [this](std::thread& thread, const std::string& name) {
        this->set_thread_name(thread, name);
    };
    get_thread_name_cb = [this](std::thread& thread) {
        return this->get_thread_name(thread);
    };
}

void set_thread_name(std::thread& thread, const std::string& name)
{
    set_thread_name_cb(thread, name);
}

std::string get_thread_name(std::thread& thread)
{
    return get_thread_name_cb(thread);
}

bool set_thread_affinity(const std::size_t cpu)
{
    return set_thread_affinity_cb(cpu);
}

}  // namespace os
}  // namespace score
