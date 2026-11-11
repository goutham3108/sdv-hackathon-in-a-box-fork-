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
#include "score/language/safecpp/scoped_function/details/call_and_return_modified.h"

#include <score/blank.hpp>
#include <score/utility.hpp>

#include <gtest/gtest.h>

#include <cstdint>

namespace score::safecpp::details
{
namespace
{

class CallAndReturnModifiedTest : public ::testing::Test
{
};

TEST_F(CallAndReturnModifiedTest, ReturnsType)
{
    const std::int32_t difference{2};
    auto lambda = [](std::int32_t value) noexcept -> std::int32_t {
        return value + difference;
    };

    const std::int32_t provided_value{13};
    const std::int32_t expected_value{provided_value + difference};

    const std::int32_t actual_value{CallAndReturnModified(lambda, provided_value)};
    EXPECT_EQ(actual_value, expected_value);
}

TEST_F(CallAndReturnModifiedTest, ReturnsWrappedReference)
{
    const std::int32_t difference{2};
    auto lambda = [](std::int32_t& value) noexcept -> std::int32_t& {
        value += difference;
        return value;
    };

    std::int32_t provided_value{13};
    const std::int32_t expected_value{provided_value + difference};

    std::reference_wrapper<std::int32_t> actual_value{CallAndReturnModified(lambda, provided_value)};
    EXPECT_EQ(actual_value.get(), expected_value);
    EXPECT_EQ(provided_value, expected_value);
}

TEST_F(CallAndReturnModifiedTest, ReturnsUniversalReference)
{
    const std::int32_t difference{2};
    auto lambda = [](std::int32_t&& value) noexcept -> std::int32_t&& {
        value += difference;
        return std::move(value);
    };

    std::int32_t provided_value{13};
    const std::int32_t expected_value{provided_value + difference};

    std::int32_t actual_value{CallAndReturnModified(lambda, std::move(provided_value))};
    EXPECT_EQ(actual_value, expected_value);
}

TEST_F(CallAndReturnModifiedTest, ReturnsAmpBlank)
{
    const std::int32_t difference{2};
    auto lambda = [](std::int32_t& value) noexcept -> void {
        value += difference;
    };

    std::int32_t provided_value{13};
    const std::int32_t expected_value{provided_value + difference};

    score::cpp::blank blank_result{CallAndReturnModified(lambda, provided_value)};
    score::cpp::ignore = blank_result;

    EXPECT_EQ(provided_value, expected_value);
}

}  // namespace
}  // namespace score::safecpp::details
