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
#include "score/os/net_if.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <thread>
#include <unordered_map>

namespace score::os
{
namespace test
{

using namespace ::testing;

TEST(NetIfTest, InstanceShouldReturnSoleObject)
{
    NetIf& subject = NetIf::instance();
    NetIf& another_subject = NetIf::instance();
    std::atomic<NetIf*> subject_from_another_thread{nullptr};
    std::thread t{[&subject_from_another_thread]() noexcept {
        NetIf* subject_ptr = &NetIf::instance();
        subject_from_another_thread.store(subject_ptr);
    }};
    t.join();

    EXPECT_THAT(&subject, NotNull());
    EXPECT_EQ(&subject, &another_subject);
    EXPECT_EQ(&subject, subject_from_another_thread.load());
}

score::cpp::expected<std::unordered_map<std::string, int>, Error> GetIfIndexes()
{

    std::unordered_map<std::string, int> result;
    struct if_nameindex* if_nidxs = if_nameindex();

    if (if_nidxs == NULL)
    {
        auto err = score::cpp::make_unexpected(Error::createFromErrno());
        return err;
    }

    for (struct if_nameindex* intf = if_nidxs; intf->if_index != 0 || intf->if_name != NULL; intf++)
    {
        result.emplace(std::string{intf->if_name}, intf->if_index);
    }

    if_freenameindex(if_nidxs);
    return result;
}

TEST(NetIfTest, ShouldReturnCorrectNetworkInterfaceIndex)
{
    NetIf& subject = NetIf::instance();

    auto adapters = GetIfIndexes();
    if (!adapters.has_value())
    {
        GTEST_FATAL_FAILURE_("failed to get network interfaces to continue testing.");
    }

    for (auto i : adapters.value())
    {
        auto result = subject.if_nametoindex(i.first);
        ASSERT_TRUE(result.has_value());
#if defined(__linux__)
        EXPECT_EQ(result.value(), i.second);
#endif
    }
}

TEST(NetIfTest, ShouldReturnErrorOnNonexistingIf)
{
    NetIf& subject = NetIf::instance();
    auto result = subject.if_nametoindex(std::string{"some non-existing network interfaces."});
    ASSERT_FALSE(result.has_value());
#if defined(__linux__)
    EXPECT_EQ(result.error(), Error::createFromErrno(ENODEV));
#endif
}

TEST(NetIfTest, ShouldReturnErrorOnLongIfName)
{
    NetIf& subject = NetIf::instance();
    std::string long_ifname(IFNAMSIZ + 10, 'h');  // Create a name longer than IFNAMSIZ

    auto result = subject.if_nametoindex(long_ifname);
    ASSERT_FALSE(result.has_value());
#if defined(__linux__)
    EXPECT_EQ(result.error(), Error::createFromErrno(ENODEV));
#endif
}

TEST(NetIfTest, ShouldReturnErrorOnEmptyIfName)
{
    NetIf& subject = NetIf::instance();
    auto result = subject.if_nametoindex(std::string{});
    ASSERT_FALSE(result.has_value());
#if defined(__linux__)
    EXPECT_EQ(result.error(), Error::createFromErrno(ENODEV));
#endif
}

}  // namespace test
}  // namespace score::os
