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
#include "score/language/safecpp/string_view/null_termination_violation_policies.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <csignal>
#include <functional>
#include <stdexcept>

namespace score::safecpp
{
namespace
{

TEST(NullTerminationViolationPolicies, Abort)
{
    // Given the `abort` policy
    null_termination_violation_policies::abort policy{};

    // When invoking it, then immediate termination is expected
    EXPECT_EXIT(std::invoke(policy, "reason"), ::testing::KilledBySignal{SIGABRT}, "");
}

TEST(NullTerminationViolationPolicies, SetEmpty)
{
    // Given the `set_empty` policy
    null_termination_violation_policies::set_empty policy{};

    // When invoking it, then must have no effect
    EXPECT_NO_THROW(std::invoke(policy, "reason"));
}

TEST(NullTerminationViolationPolicies, ThrowException)
{
    // When invoking the `throw_exception` policy in conjunction with `std::invalid_argument`
    // Then `std::invalid_argument` is expected to get thrown
    EXPECT_THAT(
        [] {
            std::invoke(null_termination_violation_policies::throw_exception<std::invalid_argument>{}, "reason");
        },
        ::testing::ThrowsMessage<std::invalid_argument>(::testing::StrEq("reason")));

    // When invoking the `throw_exception` policy in conjunction with `std::out_of_range`
    // Then `std::out_of_range` is expected to get thrown
    EXPECT_THAT(
        [] {
            std::invoke(null_termination_violation_policies::throw_exception<std::out_of_range>{}, "other reason");
        },
        ::testing::ThrowsMessage<std::out_of_range>(::testing::StrEq("other reason")));
}

TEST(NullTerminationViolationPolicies, DefaultPolicy)
{
    // Given the default violation policy
    using default_policy = null_termination_violation_policies::default_policy;

    // When checking whether it's a valid one, then true is expected
    EXPECT_TRUE(null_termination_violation_policies::is_valid_one<default_policy>());
}

TEST(NullTerminationViolationPolicies, IsValidOne)
{
    // Given a custom policy
    struct dummy_policy
    {
        using value_type = void;
    };

    // When checking whether it's a valid one, then false is expected
    EXPECT_FALSE(null_termination_violation_policies::is_valid_one<dummy_policy>());

    // When checking whether a non-class type a valid one, then false is expected
    EXPECT_FALSE(null_termination_violation_policies::is_valid_one<unsigned char>());
}

}  // namespace
}  // namespace score::safecpp
