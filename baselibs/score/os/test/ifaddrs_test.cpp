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
#include "score/os/ifaddrs.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdio>
#include <thread>

namespace score::os
{
namespace test
{

using namespace ::testing;

TEST(IfAddrsTest, InstanceShouldReturnSoleObject)
{
    Ifaddrs& subject = Ifaddrs::instance();
    Ifaddrs& another_subject = Ifaddrs::instance();
    std::atomic<Ifaddrs*> subject_from_another_thread{nullptr};
    std::thread t{[&subject_from_another_thread]() noexcept {
        Ifaddrs* subject_ptr = &Ifaddrs::instance();
        subject_from_another_thread.store(subject_ptr);
    }};
    t.join();

    EXPECT_THAT(&subject, NotNull());
    EXPECT_EQ(&subject, &another_subject);
    EXPECT_EQ(&subject, subject_from_another_thread.load());
}

}  // namespace test
}  // namespace score::os
