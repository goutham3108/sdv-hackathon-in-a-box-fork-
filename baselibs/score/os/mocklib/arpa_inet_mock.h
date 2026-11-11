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
#ifndef SCORE_LIB_OS_MOCKLIB_ARPA_INET_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_ARPA_INET_MOCK_H

#include "score/os/arpa_inet.h"
#include "score/os/errno.h"
#include "score/os/netinet_in.h"

#include <gmock/gmock.h>
#include <cstdint>

namespace score
{
namespace os
{

class ArpaInetMock : public ArpaInet
{
  public:
    MOCK_METHOD((InAddrT), InetAddr, (const std::string&), (const, override));
    // @todo: use score::Result instead of score::cpp::expected after merging this ticket(Ticket-61178).
    MOCK_METHOD((score::cpp::expected<InAddr, Error::Code>), InetPton, (const std::string&), (const, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_ARPA_INET_MOCK_H
