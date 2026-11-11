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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_PROCMGR_DAEMON_RAW_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_PROCMGR_DAEMON_RAW_H

#include <gmock/gmock.h>
#include <sys/procmgr.h>

namespace score
{
namespace os
{

/// Mock for the raw QNX ::procmgr_daemon C API.
///
/// Use SetProcMgrDaemonRawMock() to install an instance before calling any code
/// that reaches ProcMgrImpl::procmgr_daemon, and to clear it afterwards.
/// The link-time seam in mock_procmgr_daemon_raw.cpp replaces the real
/// ::procmgr_daemon symbol with a forwarder to the installed mock object,
/// so ProcMgrImpl is exercised end-to-end while the QNX syscall is controlled.
class MockProcMgrDaemonRaw
{
  public:
    MOCK_METHOD(int, procmgr_daemon, (int status, unsigned flags), (noexcept));
    MOCK_METHOD(int, procmgr_event_notify_add, (unsigned flags, const struct sigevent* event), (noexcept));
    MOCK_METHOD(int, procmgr_event_notify_delete, (int id), (noexcept));
};

/// Install (or clear with nullptr) the mock that ::procmgr_daemon forwards to.
void SetProcMgrDaemonRawMock(MockProcMgrDaemonRaw* mock) noexcept;

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_PROCMGR_DAEMON_RAW_H
