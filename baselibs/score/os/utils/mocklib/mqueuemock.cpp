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
#include "score/os/utils/mocklib/mqueuemock.h"

#include <chrono>
#include <functional>

namespace score
{
namespace os
{

class MQueue::MQueuePrivate
{
};

namespace
{

std::function<void(std::size_t)> cTor1_call;
std::function<void(const std::string&)> cTorStr_call;
std::function<void(const std::string&, AccessMode, size_t, size_t)> cTor4_call;
std::function<void(const std::string&)> send_call;
std::function<score::cpp::expected_blank<score::os::Error>(const char*, size_t)> send2_call;
std::function<ssize_t(const char*, size_t, std::chrono::milliseconds)> timed_send_call;
std::function<std::string()> receive_call;
std::function<score::cpp::expected_blank<score::os::Error>()> unlink_call;
std::function<size_t()> get_id_call;
std::function<std::pair<std::string, bool>(std::chrono::milliseconds)> timed_receive_call;
std::function<std::pair<ssize_t, bool>(char*, std::chrono::milliseconds)> timed_receive2_call;
std::function<score::cpp::expected<std::uint32_t, Error>()> get_mq_st_mode_call;

} /* namespace */

MQueueMock::MQueueMock()
{
    cTor1_call = [this](std::size_t s) {
        this->cTor1(s);
    };
    cTorStr_call = [this](const std::string& name) {
        this->cTorStr(name);
    };
    cTor4_call = [this](const std::string& name, AccessMode mode, size_t max_msg_size, size_t max_msgs) {
        this->cTor4(name, mode, max_msg_size, max_msgs);
    };
    send_call = [this](const std::string& msg) {
        this->send(msg);
    };
    send2_call = [this](const char* msg, size_t length) {
        return this->send2(msg, length);
    };
    timed_send_call = [this](const char* msg, size_t length, std::chrono::milliseconds ms) {
        return this->timed_send(msg, length, ms);
    };
    receive_call = [this]() {
        return this->receive();
    };
    unlink_call = [this]() {
        return this->unlink();
    };
    get_id_call = [this]() {
        return this->get_id();
    };
    timed_receive_call = [this](std::chrono::milliseconds ms) {
        return this->timed_receive(ms);
    };
    timed_receive2_call = [this](char* msg, std::chrono::milliseconds ms) {
        return this->timed_receive2(msg, ms);
    };
    get_mq_st_mode_call = [this]() {
        return this->get_mq_st_mode();
    };
}

std::pair<std::string, bool> MQueue::timed_receive(std::chrono::milliseconds timeout) const
{
    return timed_receive_call(timeout);
}

std::pair<ssize_t, bool> MQueue::timed_receive(char* msg, std::chrono::milliseconds timeout) const
{
    return timed_receive2_call(msg, timeout);
}

MQueue::MQueue(const std::string& name, AccessMode mode, size_t max_msg_size, size_t max_msgs)
{
    cTor4_call(name, mode, max_msg_size, max_msgs);
}

MQueue::MQueue(const std::string& name)
{
    cTorStr_call(name);
}

MQueue::MQueue(size_t) {}

MQueue::~MQueue() {}

void MQueue::send(const std::string& msg) const
{
    send_call(msg);
}

score::cpp::expected_blank<score::os::Error> MQueue::send(const char* msg, size_t length) const
{
    return send2_call(msg, length);
}

ssize_t MQueue::timed_send(const char* msg, size_t length, std::chrono::milliseconds timeout) const
{
    return timed_send_call(msg, length, timeout);
}

std::string MQueue::receive() const
{
    return receive_call();
}

score::cpp::expected_blank<score::os::Error> MQueue::unlink() const
{
    return unlink_call();
}

size_t MQueue::get_id() const
{
    return get_id_call();
}

score::cpp::expected<std::uint32_t, Error> MQueue::get_mq_st_mode() const noexcept
{
    return get_mq_st_mode_call();
}

}  // namespace os
}  // namespace score
