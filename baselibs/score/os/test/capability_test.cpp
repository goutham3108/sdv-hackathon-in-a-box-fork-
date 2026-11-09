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
#include <gtest/gtest.h>
#include <vector>

#include "score/os/mocklib/capability_mock.h"
#include "score/os/mocklib/unistdmock.h"

using namespace ::score::os;

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::ByMove;
using ::testing::Invoke;
using ::testing::Return;

class PadLinuxCapTest : public ::testing::Test
{
  public:
    PadLinuxCapTest() {}
    ~PadLinuxCapTest() override {}

  protected:
    virtual void SetUp() override
    {
        ProcessCapabilities::set_testing_instance(linuxcapmock_);
    }
    virtual void TearDown() override {};

    ProcessCapabilitiesMock linuxcapmock_;
};

TEST_F(PadLinuxCapTest, DropCapabilityKeepAll)
{
    std::vector<Capability> keep_caps = {
        Capability::SysAdmin,      Capability::Kill,       Capability::Chown,          Capability::DacOverride,
        Capability::DacReadSearch, Capability::Fowner,     Capability::Fsetid,         Capability::Setgid,
        Capability::Setuid,        Capability::Setpcap,    Capability::LinuxImmutable, Capability::NetBindService,
        Capability::NetBroadCast,  Capability::NetAdmin,   Capability::NetRaw,         Capability::IpcLock,
        Capability::IpcOwner,      Capability::SysModule,  Capability::SysRawio,       Capability::SysChroot,
        Capability::SysPtrace,     Capability::SysPacct,   Capability::Sysboot,        Capability::SysNice,
        Capability::SysResource,   Capability::SysTime,    Capability::SysTtyConfig,   Capability::Mknod,
        Capability::Lease,         Capability::AuditWrite, Capability::AuditControl,   Capability::Setfcap,
        Capability::MacOverride,   Capability::MacAdmin,   Capability::Syslog,         Capability::WakeAlarm,
        Capability::BlockSuspend,  Capability::AuditRead};
    EXPECT_CALL(linuxcapmock_, GetProcessCapabilitySets()).WillRepeatedly(Invoke([] {
        ProcessCapabilitySets caps(static_cast<void*>(reinterpret_cast<int*>(0x01)), [](void*) {});
        return caps;
    }));
    ASSERT_TRUE(linuxcapmock_.ReduceProcessCapabilitiesTo(keep_caps));
}

TEST_F(PadLinuxCapTest, DropCapabilityDropAll)
{
    std::vector<Capability> keep_caps = {};
    EXPECT_CALL(linuxcapmock_, DropCapabilityFromBoundingSet(_)).Times(AnyNumber());
    EXPECT_CALL(linuxcapmock_, GetProcessCapabilitySets()).WillRepeatedly(Invoke([] {
        ProcessCapabilitySets caps(static_cast<void*>(reinterpret_cast<int*>(0x01)), [](void*) {});
        return caps;
    }));
    EXPECT_CALL(linuxcapmock_, SetCapabilitiesInCapabilitySet(_, score::os::CapabilitySets::Permitted, _, _)).Times(1);
    EXPECT_CALL(linuxcapmock_, SetCapabilitiesInCapabilitySet(_, score::os::CapabilitySets::Effective, _, _)).Times(1);
    EXPECT_CALL(linuxcapmock_, SetProcessCapabilitySets(_)).Times(1);
    ASSERT_TRUE(linuxcapmock_.ReduceProcessCapabilitiesTo(keep_caps));
}

TEST_F(PadLinuxCapTest, DropCapabilityKeepOne)
{
    std::vector<Capability> keep_caps = {Capability::SysRawio};
    EXPECT_CALL(linuxcapmock_, DropCapabilityFromBoundingSet(_)).Times(AnyNumber());
    EXPECT_CALL(linuxcapmock_, GetProcessCapabilitySets()).WillRepeatedly(Invoke([] {
        ProcessCapabilitySets caps(static_cast<void*>(reinterpret_cast<int*>(0x01)), [](void*) {});
        return caps;
    }));
    EXPECT_CALL(linuxcapmock_, SetCapabilitiesInCapabilitySet(_, CapabilitySets::Permitted, _, _)).Times(1);
    EXPECT_CALL(linuxcapmock_, SetCapabilitiesInCapabilitySet(_, CapabilitySets::Effective, _, _)).Times(2);
    EXPECT_CALL(linuxcapmock_, SetProcessCapabilitySets(_)).Times(2);
    ASSERT_TRUE(linuxcapmock_.ReduceProcessCapabilitiesTo(keep_caps));
}

TEST_F(PadLinuxCapTest, DropCapabilityCapSetProcFails)
{
    std::vector<Capability> keep_caps = {};
    EXPECT_CALL(linuxcapmock_, DropCapabilityFromBoundingSet(_)).Times(AnyNumber());
    EXPECT_CALL(linuxcapmock_, GetProcessCapabilitySets()).WillRepeatedly(Invoke([] {
        ProcessCapabilitySets caps(static_cast<void*>(reinterpret_cast<int*>(0x01)), [](void*) {});
        return caps;
    }));
    EXPECT_CALL(linuxcapmock_, SetCapabilitiesInCapabilitySet(_, CapabilitySets::Permitted, _, _));
    EXPECT_CALL(linuxcapmock_, SetCapabilitiesInCapabilitySet(_, CapabilitySets::Effective, _, _));
    EXPECT_CALL(linuxcapmock_, SetProcessCapabilitySets(_))
        .WillOnce(Return(score::cpp::make_unexpected(Error::createFromErrno(-1))));
    ASSERT_FALSE(linuxcapmock_.ReduceProcessCapabilitiesTo(keep_caps));
}

TEST_F(PadLinuxCapTest, DropCapabilityCapDropBoundFails)
{
    std::vector<Capability> keep_caps = {};
    EXPECT_CALL(linuxcapmock_, DropCapabilityFromBoundingSet(_))
        .WillOnce(Return(score::cpp::make_unexpected(Error::createFromErrno(-1))));
    ASSERT_FALSE(linuxcapmock_.ReduceProcessCapabilitiesTo(keep_caps));
}

TEST_F(PadLinuxCapTest, DropCapabilityRaiseCapSetProcFails)
{
    std::vector<Capability> keep_caps = {Capability::Setgid, Capability::Setuid};
    EXPECT_CALL(linuxcapmock_, DropCapabilityFromBoundingSet(_)).Times(AnyNumber());
    EXPECT_CALL(linuxcapmock_, GetProcessCapabilitySets()).WillRepeatedly(Invoke([] {
        ProcessCapabilitySets caps(static_cast<void*>(reinterpret_cast<int*>(0x01)), [](void*) {});
        return caps;
    }));
    EXPECT_CALL(linuxcapmock_, SetCapabilitiesInCapabilitySet(_, _, _, _)).Times(AnyNumber());
    EXPECT_CALL(linuxcapmock_, SetProcessCapabilitySets(_))
        .WillOnce(Return(score::cpp::expected_blank<Error>()))
        .WillOnce(Return(score::cpp::make_unexpected(Error::createFromErrno(-1))));
    ASSERT_FALSE(linuxcapmock_.ReduceProcessCapabilitiesTo(keep_caps));
}

TEST_F(PadLinuxCapTest, DropCapabilityRemoveTempCapSetProcFails)
{
    std::vector<Capability> keep_caps = {Capability::SysRawio};
    EXPECT_CALL(linuxcapmock_, DropCapabilityFromBoundingSet(_)).Times(AnyNumber());
    EXPECT_CALL(linuxcapmock_, GetProcessCapabilitySets()).WillRepeatedly(Invoke([] {
        ProcessCapabilitySets caps(static_cast<void*>(reinterpret_cast<int*>(0x01)), [](void*) {});
        return caps;
    }));
    EXPECT_CALL(linuxcapmock_, SetCapabilitiesInCapabilitySet(_, CapabilitySets::Permitted, _, _)).Times(1);
    EXPECT_CALL(linuxcapmock_, SetCapabilitiesInCapabilitySet(_, CapabilitySets::Effective, _, _)).Times(2);
    EXPECT_CALL(linuxcapmock_, SetProcessCapabilitySets(_))
        .WillOnce(Return(score::cpp::expected_blank<Error>()))
        .WillOnce(Return(score::cpp::make_unexpected(Error::createFromErrno(-1))));
    ASSERT_FALSE(linuxcapmock_.ReduceProcessCapabilitiesTo(keep_caps));
}
