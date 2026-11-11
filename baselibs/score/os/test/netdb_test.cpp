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
#include "score/os/netdb.h"

#include "score/os/arpa_inet.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <thread>

namespace score::os
{
namespace test
{

using namespace ::testing;

TEST(NetdbTest, InstanceShouldReturnSoleObject)
{
    Netdb& subject = Netdb::instance();
    Netdb& another_subject = Netdb::instance();
    std::atomic<Netdb*> subject_from_another_thread{nullptr};
    std::thread t{[&subject_from_another_thread]() noexcept {
        Netdb* subject_ptr = &Netdb::instance();
        subject_from_another_thread.store(subject_ptr);
    }};
    t.join();

    EXPECT_THAT(&subject, NotNull());
    EXPECT_EQ(&subject, &another_subject);
    EXPECT_EQ(&subject, subject_from_another_thread.load());
}

struct NetdbTestFixture : public Test
{
    void SetUp() override {}

    void CallGetNameInfoAndExtractResultWithPort80(const std::string& addr_str,
                                                   Netdb::NameFlag flag,
                                                   bool expect_success = true)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(80);  // Port 80 (HTTP)
        auto& inet = ArpaInet::instance();
        auto result = inet.InetPton(addr_str);
        ASSERT_TRUE(result.has_value());
        addr_.sin_addr = result.value();

        CallGetNameInfoAndExtractResult(&addr_, flag, expect_success);
    }

    void CallGetNameInfoAndExtractResult(sockaddr_in* addr, Netdb::NameFlag flag, bool expect_success = true)
    {
        result_ = subject_.getnameinfo(reinterpret_cast<sockaddr*>(addr),
                                       sizeof(*addr),
                                       host_buffer_.data(),
                                       static_cast<unsigned int>(host_buffer_.size()),
                                       service_buffer_.data(),
                                       static_cast<unsigned int>(service_buffer_.size()),
                                       flag);
        if (expect_success)
        {
            ASSERT_TRUE(result_.has_value());
            host_ = &host_buffer_.front();
            service_ = &service_buffer_.front();
        }
        else
        {
            ASSERT_FALSE(result_.has_value());
        }
    }

  public:
    Netdb& subject_ = Netdb::instance();
    sockaddr_in addr_;
    std::array<char, NI_MAXHOST> host_buffer_;
    std::array<char, NI_MAXSERV> service_buffer_;
    std::string_view host_;
    std::string_view service_;
    score::cpp::expected_blank<Error> result_;
};

TEST_F(NetdbTestFixture, ShouldReturnHostnameAndService)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(80);                  // Port 80 (HTTP)
    addr_.sin_addr.s_addr = htonl(0x7f000001u);  // 127.0.0.1

    constexpr std::string_view expected_host{
#if defined(__linux__)
        "localhost"
#elif defined(__QNX__)
        "127.0.0.1"
#endif
    };
    constexpr std::string_view expected_service{
#if defined(__linux__)
        "http"
#elif defined(__QNX__)
        "80"
#endif
    };
    constexpr std::string_view expected_service_numeric{"80"};

    CallGetNameInfoAndExtractResult(&addr_, Netdb::NameFlag{0});
    EXPECT_EQ(host_, expected_host);
    EXPECT_THAT(service_, AnyOf(Eq(expected_service), Eq(expected_service_numeric)));
    CallGetNameInfoAndExtractResult(&addr_, Netdb::NameFlag::kNodeNameOfDomain);
    EXPECT_EQ(host_, expected_host);
    EXPECT_THAT(service_, AnyOf(Eq(expected_service), Eq(expected_service_numeric)));
#if defined(__linux__)
    CallGetNameInfoAndExtractResult(&addr_, Netdb::NameFlag::kNameReq);
    EXPECT_EQ(host_, expected_host);
    EXPECT_THAT(service_, AnyOf(Eq(expected_service), Eq(expected_service_numeric)));
#elif defined(__QNX__)
    CallGetNameInfoAndExtractResult(&addr_, Netdb::NameFlag::kNameReq, false);
#endif
}

TEST_F(NetdbTestFixture, ShouldReturnNumbers)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(80);                  // Port 80 (HTTP)
    addr_.sin_addr.s_addr = htonl(0x7f000001u);  // 127.0.0.1

    CallGetNameInfoAndExtractResult(&addr_, Netdb::NameFlag::kNumericHost | Netdb::NameFlag::kNumericService);
    EXPECT_EQ(host_, "127.0.0.1");
    EXPECT_EQ(service_, "80");
}

TEST_F(NetdbTestFixture, ShouldFailOnInvalidArguments)
{
    addr_.sin_family = 223;                      // Invalid family
    addr_.sin_port = htons(80);                  // Port 80 (HTTP)
    addr_.sin_addr.s_addr = htonl(0x0a000001u);  // 10.0.0.1

    result_ = subject_.getnameinfo(
        reinterpret_cast<sockaddr*>(&addr_), 0U, nullptr, 0U, nullptr, 0U, Netdb::NameFlag::kNameReq);
    ASSERT_FALSE(result_.has_value());
}

TEST_F(NetdbTestFixture, UdpResolveTest)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(53);                  // Port 53 (DNS)
    addr_.sin_addr.s_addr = htonl(0x7f000001u);  // 127.0.0.1

    constexpr std::string_view expected_host{
#if defined(__linux__)
        "localhost"
#elif defined(__QNX__)
        "127.0.0.1"
#endif
    };
    constexpr std::string_view expected_service{
#if defined(__linux__)
        "domain"
#elif defined(__QNX__)
        "53"
#endif
    };
    constexpr std::string_view expected_service_numeric{"53"};

    CallGetNameInfoAndExtractResult(&addr_, Netdb::NameFlag::kDatagramService | Netdb::NameFlag::kNumericHost);
    EXPECT_EQ(host_, "127.0.0.1");
    EXPECT_THAT(service_, AnyOf(Eq(expected_service), Eq(expected_service_numeric)));

    CallGetNameInfoAndExtractResultWithPort80("127.0.0.1", Netdb::NameFlag::kDatagramService);
    EXPECT_EQ(host_, expected_host);
    EXPECT_EQ(service_, "80");  // http is based on TCP
}

}  // namespace test
}  // namespace score::os
