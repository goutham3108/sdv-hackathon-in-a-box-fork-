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
#include "score/os/arpa_inet.h"

#include "score/os/errno.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fcntl.h>
#include <grp.h>
#include <netinet/in.h>
#include <thread>

namespace score::os
{
namespace test
{
using namespace ::testing;

TEST(ArpaInetTest, InstanceShouldReturnSoleObject)
{
    ArpaInet& subject = ArpaInet::instance();
    ArpaInet& another_subject = ArpaInet::instance();
    std::atomic<ArpaInet*> subject_from_another_thread{nullptr};
    std::thread t{[&subject_from_another_thread]() noexcept {
        ArpaInet* subject_ptr = &ArpaInet::instance();
        subject_from_another_thread.store(subject_ptr);
    }};
    t.join();

    EXPECT_THAT(&subject, NotNull());
    EXPECT_EQ(&subject, &another_subject);
    EXPECT_EQ(&subject, subject_from_another_thread.load());
}

typedef std::pair<std::string, in_addr_t> InetAddrTestCaseT;
struct ArpaInetInetAddrFixture : public TestWithParam<InetAddrTestCaseT>
{
    ArpaInet& instance_ = ArpaInet::instance();
};

auto valid_cases = ::testing::Values(InetAddrTestCaseT{"0.0.0.0", 0u},
                                     InetAddrTestCaseT{"128.128.128.128", 0x80808080u},
                                     InetAddrTestCaseT{"127.127.127.127", 0x7f7f7f7fu},
                                     InetAddrTestCaseT{"114.114.114.114", 0x72727272u},
                                     InetAddrTestCaseT{"192.168.31.1", htonl(0xc0a81f01u)},
                                     InetAddrTestCaseT{"1.2.3.4", htonl(0x01020304u)},
                                     InetAddrTestCaseT{"4.3.2.1", htonl(0x04030201u)});
auto inet_addr_valid_cases = ::testing::Values(InetAddrTestCaseT{"1", htonl(0x1u)},
                                               InetAddrTestCaseT{"65535", htonl(0x0000ffffu)},
                                               InetAddrTestCaseT{"65536", htonl(0x00010000u)},
                                               InetAddrTestCaseT{"1.2", htonl(0x01000002u)},
                                               InetAddrTestCaseT{"1.2.3", htonl(0x01020003u)},
                                               InetAddrTestCaseT{"0xffff", htonl(0x0000ffffu)},
                                               InetAddrTestCaseT{"0x10000", htonl(0x00010000u)},
                                               InetAddrTestCaseT{"255.255.255.255", htonl(0xffffffffu)},
                                               InetAddrTestCaseT{"0xFFFFFFFF", htonl(0xffffffffu)},
                                               InetAddrTestCaseT{"4294967295", htonl(0xffffffffu)});
auto inet_pton_invalid_cases = ::testing::Values(InetAddrTestCaseT{"1", INADDR_NONE},
                                                 InetAddrTestCaseT{"65535", INADDR_NONE},
                                                 InetAddrTestCaseT{"65536", INADDR_NONE},
                                                 InetAddrTestCaseT{"1.2", INADDR_NONE},
                                                 InetAddrTestCaseT{"1.2.3", INADDR_NONE},
                                                 InetAddrTestCaseT{"0xffff", INADDR_NONE},
                                                 InetAddrTestCaseT{"0x10000", INADDR_NONE});
auto invalid_cases = ::testing::Values(InetAddrTestCaseT{"999.998.997.996", INADDR_NONE},
                                       InetAddrTestCaseT{"-11.2.3.3", INADDR_NONE},
                                       InetAddrTestCaseT{"1.2.3.abc", INADDR_NONE},
                                       InetAddrTestCaseT{"1,2,3,4", INADDR_NONE},
                                       InetAddrTestCaseT{"1.2.3.654", INADDR_NONE},
                                       InetAddrTestCaseT{"abc.bcd.def.fgh", INADDR_NONE},
                                       InetAddrTestCaseT{"1.2.abc.5", INADDR_NONE},
                                       InetAddrTestCaseT{"1.2.999.5", INADDR_NONE},
                                       InetAddrTestCaseT{"1.2222.5.6", INADDR_NONE},
                                       InetAddrTestCaseT{"1.abpp.4.6", INADDR_NONE},
                                       InetAddrTestCaseT{"123456.7.8.9", INADDR_NONE},
                                       InetAddrTestCaseT{"abcdefg.7.6.5", INADDR_NONE},
                                       InetAddrTestCaseT{"1.2.3.4.5.6.7", INADDR_NONE},
                                       InetAddrTestCaseT{"112.113.114.115.", INADDR_NONE},
                                       InetAddrTestCaseT{".12.32.43.54", INADDR_NONE},
                                       InetAddrTestCaseT{"i.m.a.f.i.s.h", INADDR_NONE},
                                       InetAddrTestCaseT{"hank.guo@example.com", INADDR_NONE});

INSTANTIATE_TEST_SUITE_P(ValidAddresses, ArpaInetInetAddrFixture, valid_cases);
INSTANTIATE_TEST_SUITE_P(InetAddrValidAddresses, ArpaInetInetAddrFixture, inet_addr_valid_cases);
INSTANTIATE_TEST_SUITE_P(InetAddrInvalidIpAddresses, ArpaInetInetAddrFixture, invalid_cases);

TEST_P(ArpaInetInetAddrFixture, InetAddrShouldReturnExpectedValue)
{
    const auto& testcase = GetParam();

    EXPECT_EQ(instance_.InetAddr(testcase.first), testcase.second);
}

struct ArpaInetInetPtonFixture : public TestWithParam<InetAddrTestCaseT>
{
    ArpaInet& instance_ = ArpaInet::instance();
};
INSTANTIATE_TEST_SUITE_P(InetPtonValidAddresses, ArpaInetInetPtonFixture, valid_cases);
INSTANTIATE_TEST_SUITE_P(InvalidIpAddresses, ArpaInetInetPtonFixture, invalid_cases);
INSTANTIATE_TEST_SUITE_P(InetPtonInvalidIpAddresses, ArpaInetInetPtonFixture, inet_pton_invalid_cases);

TEST_P(ArpaInetInetPtonFixture, InetPtonShouldReturnExpectedValue)
{
    const auto& testcase = GetParam();
    const auto& address = testcase.first;
    const auto& addr_t = testcase.second;

    auto result = instance_.InetPton(address);

    if (addr_t == INADDR_NONE)
    {
        ASSERT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), Error::Code::kInvalidArgument);
    }
    else
    {
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value().s_addr, addr_t);
    }
}

}  // namespace test

}  // namespace score::os
