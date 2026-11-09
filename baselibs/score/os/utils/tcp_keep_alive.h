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
#ifndef SCORE_LIB_OS_UTILS_TCP_KEEP_ALIVE_H
#define SCORE_LIB_OS_UTILS_TCP_KEEP_ALIVE_H
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>

namespace score
{
namespace os
{

/// @brief This function will enable the periodic keep_alive messages of TCP.
///        If the TCP connection is broken because of unplugged cabels, an error
///        will be generated.
///
/// @param sockfd  the respected socket referred to by the file descriptor sockfd.
bool TcpKeepAlive(int sockfd);

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_TCP_KEEP_ALIVE_H
