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
#ifndef SCORE_LIB_OS_UTILS_MOCKLIB_MQUEUEMOCK_H
#define SCORE_LIB_OS_UTILS_MOCKLIB_MQUEUEMOCK_H

#include "score/os/errno.h"
#include "score/os/utils/mqueue.h"
#include <score/expected.hpp>

#include <gmock/gmock.h>
#include <cstdint>

namespace score
{
namespace os
{

class MQueueMock
{
  public:
    MQueueMock();
    MOCK_METHOD(void, cTor1, (std::size_t id));
    MOCK_METHOD(void, cTorStr, (const std::string& name));
    MOCK_METHOD(void, cTor4, (const std::string& name, AccessMode mode, size_t max_msg_size, size_t max_msgs));
    MOCK_METHOD(void, send, (const std::string& msg));
    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, send2, (const char*, size_t));
    MOCK_METHOD(ssize_t, timed_send, (const char*, size_t, std::chrono::milliseconds));
    MOCK_METHOD(std::string, receive, ());
    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, unlink, ());
    MOCK_METHOD(size_t, get_id, ());
    using StringAndBool = std::pair<std::string, bool>;
    MOCK_METHOD(StringAndBool, timed_receive, (std::chrono::milliseconds timeout));
    using SSizetAndBool = std::pair<ssize_t, bool>;
    MOCK_METHOD(SSizetAndBool, timed_receive2, (char*, std::chrono::milliseconds));
    using SUintAndError = score::cpp::expected<std::uint32_t, Error>;
    MOCK_METHOD(SUintAndError, get_mq_st_mode, ());
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_MOCKLIB_MQUEUEMOCK_H
