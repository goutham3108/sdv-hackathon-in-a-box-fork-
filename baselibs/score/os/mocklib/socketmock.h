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
#ifndef SCORE_LIB_OS_MOCKLIB_SOCKETMOCK_H
#define SCORE_LIB_OS_MOCKLIB_SOCKETMOCK_H

#include "score/os/socket.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class SocketMock : public Socket
{
  public:
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                socket,
                (const Domain, const std::int32_t, const std::int32_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                bind,
                (const std::int32_t, const struct sockaddr*, const socklen_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                listen,
                (const std::int32_t, const std::int32_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                accept,
                (const std::int32_t, struct sockaddr*, socklen_t*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                connect,
                (const std::int32_t, const struct sockaddr*, const socklen_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                setsockopt,
                (const std::int32_t, const std::int32_t, const std::int32_t, const void*, const socklen_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                getsockopt,
                (const std::int32_t, const std::int32_t, const std::int32_t, void*, socklen_t*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<ssize_t, Error>),
                recv,
                (const std::int32_t, void*, const size_t, const MessageFlag),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<ssize_t, Error>),
                recvfrom,
                (const std::int32_t, void*, const size_t, const MessageFlag, sockaddr*, socklen_t*),
                (const, noexcept, override));
    MOCK_METHOD(
        (score::cpp::expected<ssize_t, Error>),
        sendto,
        (const std::int32_t, const void*, const size_t, const MessageFlag, const struct sockaddr*, const socklen_t),
        (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<ssize_t, Error>),
                recvmsg,
                (const std::int32_t, msghdr*, const MessageFlag),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<ssize_t, Error>),
                recvmmsg,
                (const std::int32_t, mmsghdr*, const unsigned int, const MessageFlag, struct timespec*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<ssize_t, Error>),
                send,
                (const std::int32_t, void*, const size_t, const MessageFlag),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<ssize_t, Error>),
                sendmsg,
                (const std::int32_t, const msghdr*, const MessageFlag),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                sendmmsg,
                (const std::int32_t sockfd,
                 const mmsghdr* messages_array,
                 const std::uint32_t message_array_length,
                 const MessageFlag flags),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_SOCKETMOCK_H
