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
#include "score/os/mocklib/qnx/mock_procmgr_daemon_raw.h"

#include <cassert>

namespace
{
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
score::os::MockProcMgrDaemonRaw* g_procmgr_daemon_mock{nullptr};
}  // namespace

namespace score
{
namespace os
{

void SetProcMgrDaemonRawMock(MockProcMgrDaemonRaw* mock) noexcept
{
    g_procmgr_daemon_mock = mock;
}

}  // namespace os
}  // namespace score

// Link-time seam: this definition replaces ::procmgr_daemon from the QNX system
// library when this translation unit is included in the test binary.
// The linker resolves the call in ProcMgrImpl::procmgr_daemon to this symbol
// because user object files take precedence over archive members.
// NOLINTNEXTLINE(readability-redundant-declaration) – intentional redefinition
extern "C" int procmgr_daemon(int status, unsigned flags)
{
    assert(g_procmgr_daemon_mock != nullptr &&
           "procmgr_daemon called without a mock installed – call SetProcMgrDaemonRawMock() first");
    return g_procmgr_daemon_mock->procmgr_daemon(status, flags);
}

// Link-time seam: this definition replaces ::procmgr_event_notify_add from the QNX system
// library when this translation unit is included in the test binary.
// The linker resolves the call in ProcMgrImpl::procmgr_event_notify_add to this symbol
// because user object files take precedence over archive members.
// NOLINTNEXTLINE(readability-redundant-declaration) – intentional redefinition
extern "C" int procmgr_event_notify_add(unsigned flags, const struct sigevent* event)
{
    assert(g_procmgr_daemon_mock != nullptr &&
           "procmgr_event_notify_add called without a mock installed – call SetProcMgrDaemonRawMock() first");
    return g_procmgr_daemon_mock->procmgr_event_notify_add(flags, event);
}

// Link-time seam: this definition replaces ::procmgr_event_notify_delete from the QNX system
// library when this translation unit is included in the test binary.
// The linker resolves the call in ProcMgrImpl::procmgr_event_notify_delete to this symbol
// because user object files take precedence over archive members.
// NOLINTNEXTLINE(readability-redundant-declaration) – intentional redefinition
extern "C" int procmgr_event_notify_delete(int id)
{
    assert(g_procmgr_daemon_mock != nullptr &&
           "procmgr_event_notify_delete called without a mock installed – call SetProcMgrDaemonRawMock() first");
    return g_procmgr_daemon_mock->procmgr_event_notify_delete(id);
}
