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
#include "score/language/safecpp/scoped_function/details/function_wrapper.h"

#include "score/language/safecpp/scoped_function/details/instrumented_memory_resource.h"

#include <score/memory.hpp>

#include <include/gtest/gtest.h>

#include <cstdint>

namespace score::safecpp::details
{
namespace
{

class FunctionWrapperTest : public ::testing::Test
{
};

using FunctionWrapperType = FunctionWrapper<std::int32_t(std::int32_t)>;

TEST_F(FunctionWrapperTest, CallsCallable)
{
    std::int32_t provided_value{17};
    auto lambda = [](std::int32_t value) noexcept {
        return value + 22;
    };
    std::int32_t expected_value{lambda(provided_value)};
    typename FunctionWrapperType::template Implementation<decltype(lambda)> function_wrapper{lambda};

    std::int32_t received_value = function_wrapper(provided_value);
    EXPECT_EQ(received_value, expected_value);
}

TEST_F(FunctionWrapperTest, CanCopyUsingAllocator)
{
    std::int32_t provided_value{17};
    auto lambda = [](std::int32_t value) noexcept {
        return value;
    };
    std::int32_t expected_value{lambda(provided_value)};
    typename FunctionWrapperType::template Implementation<decltype(lambda)> function_wrapper{lambda};

    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<> allocator{&instrumented_memory_resource};
    auto copied_function_wrapper = function_wrapper.Copy(allocator);

    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);

    std::int32_t received_value = (*copied_function_wrapper)(provided_value);
    EXPECT_EQ(received_value, expected_value);
}

TEST_F(FunctionWrapperTest, CopyUsingAllocatorOfNonCopyableTypeCausesTermination)
{
    class MoveOnlyType
    {
      public:
        MoveOnlyType() = default;
        MoveOnlyType(const MoveOnlyType&) = delete;
        MoveOnlyType& operator=(const MoveOnlyType&) = delete;
        MoveOnlyType(MoveOnlyType&&) = default;
        MoveOnlyType& operator=(MoveOnlyType&&) = default;
        ~MoveOnlyType() = default;

        std::int32_t operator()(std::int32_t value) noexcept
        {
            return value;
        }
    };

    MoveOnlyType move_only{};
    typename FunctionWrapperType::template Implementation<decltype(move_only)> function_wrapper{std::move(move_only)};

    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<> allocator{&instrumented_memory_resource};
    EXPECT_DEATH(std::ignore = function_wrapper.Copy(allocator), ".*");
}

TEST_F(FunctionWrapperTest, CanMoveByCopyUsingAllocator)
{
    class CopyOnlyType
    {
      public:
        CopyOnlyType() = default;
        CopyOnlyType(const CopyOnlyType&) = default;
        CopyOnlyType& operator=(const CopyOnlyType&) = default;
        CopyOnlyType(CopyOnlyType&&) = delete;
        CopyOnlyType& operator=(CopyOnlyType&&) = delete;
        ~CopyOnlyType() = default;

        std::int32_t operator()(std::int32_t value) noexcept
        {
            return value;
        }
    };

    std::int32_t provided_value{17};
    CopyOnlyType copy_only{};
    std::int32_t expected_value{copy_only(provided_value)};
    typename FunctionWrapperType::template Implementation<decltype(copy_only)> function_wrapper{copy_only};

    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<> allocator{&instrumented_memory_resource};
    auto moved_function_wrapper = function_wrapper.Move(allocator);

    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);

    std::int32_t received_value = (*moved_function_wrapper)(provided_value);
    EXPECT_EQ(received_value, expected_value);
}

TEST_F(FunctionWrapperTest, CanMoveUsingAllocator)
{
    std::int32_t provided_value{17};
    auto lambda = [](std::int32_t value) noexcept {
        return value;
    };
    std::int32_t expected_value{lambda(provided_value)};
    typename FunctionWrapperType::template Implementation<decltype(lambda)> function_wrapper{lambda};

    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<> allocator{&instrumented_memory_resource};
    auto moved_function_wrapper = function_wrapper.Move(allocator);

    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);

    std::int32_t received_value = (*moved_function_wrapper)(provided_value);
    EXPECT_EQ(received_value, expected_value);
}

}  // namespace
}  // namespace score::safecpp::details
