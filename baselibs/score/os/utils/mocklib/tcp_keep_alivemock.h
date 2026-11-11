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
#ifndef SCORE_LIB_OS_UTILS_MOCKLIB_TCP_KEEP_ALIVEMOCK_H
#define SCORE_LIB_OS_UTILS_MOCKLIB_TCP_KEEP_ALIVEMOCK_H

#include <gmock/gmock.h>

#include "score/os/utils/tcp_keep_alive.h"

namespace score
{
namespace os
{

class TcpKeepAliveMocK
{
  private:
    static TcpKeepAliveMocK* instance_;

    TcpKeepAliveMocK() {};

    static void cleanUp()
    {
        delete instance_;
    }

  public:
    static TcpKeepAliveMocK& getInstance()
    {
        if (!instance_)
        {
            instance_ = new TcpKeepAliveMocK();
            atexit(cleanUp);
        }
        return *instance_;
    }
    MOCK_METHOD(bool, TcpKeepAlive, (int sockfd));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_MOCKLIB_TCP_KEEP_ALIVEMOCK_H
