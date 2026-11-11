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
#ifndef SCORE_LIB_OS_MOCKLIB_MQUEUEMOCK_H
#define SCORE_LIB_OS_MOCKLIB_MQUEUEMOCK_H

#include "score/os/mqueue.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class MqueueMock : public Mqueue
{
  public:
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                mq_open,
                (const char*, const OpenFlag, const ModeFlag, mq_attr*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                mq_open,
                (const char*, const OpenFlag),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), mq_unlink, (const char*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                mq_send,
                (const mqd_t, const char*, const size_t, const std::uint32_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                mq_timedsend,
                (const mqd_t, const char*, const size_t, const std::uint32_t, const struct timespec*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<ssize_t, Error>),
                mq_receive,
                (const mqd_t, char*, const size_t, std::uint32_t*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<ssize_t, Error>),
                mq_timedreceive,
                (const mqd_t, char*, const size_t, std::uint32_t*, const struct timespec*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), mq_close, (const mqd_t), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), mq_getattr, (const mqd_t, mq_attr&), (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_MQUEUEMOCK_H
