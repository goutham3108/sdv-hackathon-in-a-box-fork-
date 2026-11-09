/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#include "score/analysis/tracing/common/canary_wrapper/canary_wrapper.h"
#include <gtest/gtest.h>
#include <cstring>
#include <string>

namespace score
{
namespace analysis
{
namespace tracing
{

// Test with uint64_t canaries (default)
TEST(CanaryWrapperTest, DefaultConstructorInitializesCanariesUint64)
{
    const CanaryWrapper<int> wrapper;
    EXPECT_TRUE(wrapper.GetData().has_value());
    auto data = wrapper.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get(), 0);
}

// Test with uint32_t canaries
TEST(CanaryWrapperTest, DefaultConstructorInitializesCanariesUint32)
{
    const CanaryWrapper<int, std::uint32_t> wrapper;
    EXPECT_TRUE(wrapper.GetData().has_value());
    auto data = wrapper.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get(), 0);
}

TEST(CanaryWrapperTest, ConstructorForwardsSingleArgument)
{
    CanaryWrapper<int> wrapper(42);
    EXPECT_TRUE(wrapper.GetData().has_value());
    auto data = wrapper.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get(), 42);
}

TEST(CanaryWrapperTest, ConstructorForwardsMultipleArguments)
{
    struct TestStruct
    {
        int a;
        double b;
        TestStruct(int x, double y) : a(x), b(y) {}
    };

    CanaryWrapper<TestStruct> wrapper(10, 3.14);
    EXPECT_TRUE(wrapper.GetData().has_value());
    auto data = wrapper.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get().a, 10);
    EXPECT_DOUBLE_EQ(data.value().get().b, 3.14);
}

TEST(CanaryWrapperTest, ConstructorForwardsStringArgument)
{
    std::string str = "hello";
    CanaryWrapper<std::string, std::uint32_t> wrapper(str);
    EXPECT_TRUE(wrapper.GetData().has_value());
    auto data = wrapper.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get(), str);
}

TEST(CanaryWrapperTest, GetDataReturnsEmptyOptionalWhenCorrupted)
{
    CanaryWrapper<int> wrapper(42);
    EXPECT_TRUE(wrapper.GetData().has_value());

    // Verify data is accessible before corruption
    auto data_before = wrapper.GetData();
    ASSERT_TRUE(data_before.has_value());
    EXPECT_EQ(data_before.value().get(), 42);

    // Corrupt start canary
    auto* start_canary = reinterpret_cast<std::uint64_t*>(&wrapper);
    *start_canary = 0xBADBADBADBADBADULL;
    EXPECT_FALSE(wrapper.GetData().has_value());

    // GetData should return empty optional for corrupted data
    auto data_after = wrapper.GetData();
    EXPECT_FALSE(data_after.has_value());
}

TEST(CanaryWrapperTest, DetectsStartCanaryCorruption)
{
    CanaryWrapper<int> wrapper(42);
    EXPECT_TRUE(wrapper.GetData().has_value());

    // Corrupt start canary via memory access
    auto* start_canary = reinterpret_cast<std::uint64_t*>(&wrapper);
    *start_canary = 0xBADBADBADBADBADULL;
    EXPECT_FALSE(wrapper.GetData().has_value());
}

TEST(CanaryWrapperTest, DetectsEndCanaryCorruption)
{
    CanaryWrapper<int> wrapper(42);
    EXPECT_TRUE(wrapper.GetData().has_value());

    // Corrupt end canary using memcpy to handle potential misalignment
    std::uint64_t corrupted_value = 0xBADBADBADBADBADULL;
    auto* end_canary_addr = reinterpret_cast<std::uint8_t*>(&wrapper) + sizeof(std::uint64_t) + sizeof(int);
    std::memcpy(end_canary_addr, &corrupted_value, sizeof(std::uint64_t));
    EXPECT_FALSE(wrapper.GetData().has_value());
}

TEST(CanaryWrapperTest, IsCorruptedAllConditions)
{
    // Test condition 1: canary_start != kCanaryStart → true (corrupted)
    {
        CanaryWrapper<int> wrapper(42);
        auto* start_canary = reinterpret_cast<std::uint64_t*>(&wrapper);
        *start_canary = 0xBADBADBADBADBADULL;  // Corrupt start canary
        EXPECT_FALSE(wrapper.GetData().has_value()) << "Should detect start canary corruption";
    }

    // Test condition 2: canary_start != kCanaryStart → false (not corrupted, start canary is valid)
    {
        CanaryWrapper<int> wrapper(42);
        // canary_start_ is already valid (kCanaryStart)
        // canary_end_ is also valid (kCanaryEnd)
        EXPECT_TRUE(wrapper.GetData().has_value()) << "Should not detect corruption when start canary is valid";
    }

    // Test condition 3: canary_end != kCanaryEnd → true (corrupted)
    {
        CanaryWrapper<int> wrapper(42);
        std::uint64_t corrupted_value = 0xBADBADBADBADBADULL;
        auto* end_canary_addr = reinterpret_cast<std::uint8_t*>(&wrapper) + sizeof(std::uint64_t) + sizeof(int);
        std::memcpy(end_canary_addr, &corrupted_value, sizeof(std::uint64_t));
        EXPECT_FALSE(wrapper.GetData().has_value()) << "Should detect end canary corruption";
    }

    // Test condition 4: canary_end != kCanaryEnd → false (not corrupted, end canary is valid)
    {
        CanaryWrapper<int> wrapper(42);
        // Both canaries already valid
        EXPECT_TRUE(wrapper.GetData().has_value()) << "Should not detect corruption when end canary is valid";
    }

    // Both canaries valid (fully not corrupted)
    {
        CanaryWrapper<int> wrapper(42);
        EXPECT_TRUE(wrapper.GetData().has_value()) << "Should not detect corruption when both canaries are valid";
    }

    // Both canaries corrupted (fully corrupted)
    {
        CanaryWrapper<int> wrapper(42);
        auto* start_canary = reinterpret_cast<std::uint64_t*>(&wrapper);
        *start_canary = 0xDEADDEADDEADDEADULL;
        std::uint64_t end_corrupted_value = 0xBEEFBEEFBEEFBEEFULL;
        auto* end_canary_addr = reinterpret_cast<std::uint8_t*>(&wrapper) + sizeof(std::uint64_t) + sizeof(int);
        std::memcpy(end_canary_addr, &end_corrupted_value, sizeof(std::uint64_t));
        EXPECT_FALSE(wrapper.GetData().has_value()) << "Should detect corruption when both canaries are corrupted";
    }
}

TEST(CanaryWrapperTest, DetectsBothCanariesCorrupted)
{
    CanaryWrapper<int, std::uint32_t> wrapper(42);
    EXPECT_TRUE(wrapper.GetData().has_value());

    // Corrupt both canaries via memory access
    auto* start_canary = reinterpret_cast<std::uint32_t*>(&wrapper);
    auto* end_canary = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(&wrapper) +
                                                        sizeof(std::uint32_t) + sizeof(int));
    *start_canary = 0xBADBADU;
    *end_canary = 0xBADBADU;
    EXPECT_FALSE(wrapper.GetData().has_value());
}

TEST(CanaryWrapperTest, DataAccessible)
{
    CanaryWrapper<int> wrapper(100);

    // Read access
    auto data = wrapper.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get(), 100);

    // Write access
    auto data_writable = wrapper.GetData();
    ASSERT_TRUE(data_writable.has_value());
    data_writable.value().get() = 200;
    auto data_after = wrapper.GetData();
    ASSERT_TRUE(data_after.has_value());
    EXPECT_EQ(data_after.value().get(), 200);
    EXPECT_TRUE(wrapper.GetData().has_value());
}

TEST(CanaryWrapperTest, MemoryLayoutCorrect)
{
    CanaryWrapper<int, std::uint32_t> wrapper(42);

    // Verify memory layout: canary_start, data, canary_end
    auto* base = reinterpret_cast<std::uint8_t*>(&wrapper);
    auto* start_canary_ptr = reinterpret_cast<std::uint32_t*>(base);
    auto* data_ptr = reinterpret_cast<int*>(base + sizeof(std::uint32_t));
    auto* end_canary_ptr = reinterpret_cast<std::uint32_t*>(base + sizeof(std::uint32_t) + sizeof(int));

    EXPECT_EQ(*start_canary_ptr, 0xDEADBEEFU);
    EXPECT_EQ(*data_ptr, 42);
    EXPECT_EQ(*end_canary_ptr, 0xCAFEBABEU);
}

TEST(CanaryWrapperTest, ComplexTypeWrapper)
{
    struct ComplexType
    {
        int x;
        double y;
        std::string z;

        ComplexType() : x(0), y(0.0), z("") {}
        ComplexType(int a, double b, const std::string& c) : x(a), y(b), z(c) {}
    };

    std::string str = "test";
    CanaryWrapper<ComplexType> wrapper(5, 2.5, str);
    EXPECT_TRUE(wrapper.GetData().has_value());
    auto data = wrapper.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get().x, 5);
    EXPECT_DOUBLE_EQ(data.value().get().y, 2.5);
    EXPECT_EQ(data.value().get().z, "test");
}

TEST(CanaryWrapperTest, CopyConstructorPreservesDataAndRefreshesCanaries)
{
    CanaryWrapper<int> original(42);
    EXPECT_TRUE(original.GetData().has_value());

    CanaryWrapper<int> copy(original);
    EXPECT_TRUE(copy.GetData().has_value());
    auto data = copy.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get(), 42);
}

TEST(CanaryWrapperTest, CopyConstructorCopiesCorruptionState)
{
    CanaryWrapper<int> original(42);

    // Corrupt original
    auto* start_canary = reinterpret_cast<std::uint64_t*>(&original);
    *start_canary = 0xBADBADBADBADBADULL;
    EXPECT_FALSE(original.GetData().has_value());

    // Copy preserves corruption state
    CanaryWrapper<int> copy(original);
    EXPECT_FALSE(copy.GetData().has_value());
    auto data = copy.GetData();
    EXPECT_FALSE(data.has_value());
}

TEST(CanaryWrapperTest, CopyAssignmentPreservesDataAndRefreshesCanaries)
{
    CanaryWrapper<int> original(42);
    CanaryWrapper<int> copy(0);

    copy = original;
    EXPECT_TRUE(copy.GetData().has_value());
    auto data = copy.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get(), 42);
}

TEST(CanaryWrapperTest, CopyAssignmentCopiesCorruptionState)
{
    CanaryWrapper<int> original(42);
    CanaryWrapper<int> copy(0);

    // Corrupt original
    auto* start_canary = reinterpret_cast<std::uint64_t*>(&original);
    *start_canary = 0xBADBADBADBADBADULL;
    EXPECT_FALSE(original.GetData().has_value());

    // Copy assignment preserves corruption state
    copy = original;
    EXPECT_FALSE(copy.GetData().has_value());
    auto data = copy.GetData();
    EXPECT_FALSE(data.has_value());
}

TEST(CanaryWrapperTest, MoveConstructorPreservesDataAndCanaries)
{
    std::string str = "hello";
    CanaryWrapper<std::string> original(str);
    EXPECT_TRUE(original.GetData().has_value());

    CanaryWrapper<std::string> moved(std::move(original));
    EXPECT_TRUE(moved.GetData().has_value());
    auto data = moved.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get(), "hello");
}

TEST(CanaryWrapperTest, MoveAssignmentPreservesDataAndCanaries)
{
    std::string str1 = "hello";
    std::string str2 = "world";
    CanaryWrapper<std::string> original(str1);
    CanaryWrapper<std::string> moved(str2);

    moved = std::move(original);
    EXPECT_TRUE(moved.GetData().has_value());
    auto data = moved.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get(), "hello");
}

TEST(CanaryWrapperTest, MoveConstructorCopiesCorruptionState)
{
    std::string str = "hello";
    CanaryWrapper<std::string> original(str);

    // Corrupt original
    auto* start_canary = reinterpret_cast<std::uint64_t*>(&original);
    *start_canary = 0xBADBADBADBADBADULL;
    EXPECT_FALSE(original.GetData().has_value());

    // Move preserves corruption state
    CanaryWrapper<std::string> moved(std::move(original));
    EXPECT_FALSE(moved.GetData().has_value());
}

TEST(CanaryWrapperTest, MoveAssignmentCopiesCorruptionState)
{
    std::string str1 = "hello";
    std::string str2 = "world";
    CanaryWrapper<std::string> original(str1);
    CanaryWrapper<std::string> moved(str2);

    // Corrupt original
    auto* start_canary = reinterpret_cast<std::uint64_t*>(&original);
    *start_canary = 0xBADBADBADBADBADULL;
    EXPECT_FALSE(original.GetData().has_value());

    // Move assignment preserves corruption state
    moved = std::move(original);
    EXPECT_FALSE(moved.GetData().has_value());
}

TEST(CanaryWrapperTest, DetectsOnlyStartCanaryCorruptionWithValidEnd)
{
    CanaryWrapper<int> wrapper(42);

    // Verify initial state is valid
    EXPECT_TRUE(wrapper.GetData().has_value());

    // Corrupt only the start canary, leave end canary valid
    auto* start_canary = reinterpret_cast<std::uint64_t*>(&wrapper);
    *start_canary = 0xBADBADBADBADBADULL;

    // This should trigger Branch 1 (start canary corrupted)
    // and NOT evaluate the second condition due to short-circuit
    EXPECT_FALSE(wrapper.GetData().has_value());
}

TEST(CanaryWrapperTest, DetectsOnlyEndCanaryCorruptionWithValidStart)
{
    CanaryWrapper<int> wrapper(42);

    // Verify initial state is valid
    EXPECT_TRUE(wrapper.GetData().has_value());

    // Corrupt only the end canary, leave start canary valid
    std::uint64_t corrupted_value = 0xBADBADBADBADBADULL;
    auto* end_canary_addr = reinterpret_cast<std::uint8_t*>(&wrapper) + sizeof(std::uint64_t) + sizeof(int);
    std::memcpy(end_canary_addr, &corrupted_value, sizeof(std::uint64_t));

    // This should evaluate the first condition (false), then the second condition (true)
    // This hits Branch 0 (false), then Branch 3 (true - end canary corrupted)
    EXPECT_FALSE(wrapper.GetData().has_value());
}

TEST(CanaryWrapperTest, BothCanariesValidReturnsData)
{
    CanaryWrapper<int> wrapper(42);

    // Both canaries are valid - this tests the "all false" path
    // Branch 0 (start valid - false), Branch 2 (end valid - false)
    EXPECT_TRUE(wrapper.GetData().has_value());
    auto data = wrapper.GetData();
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data.value().get(), 42);
}

}  // namespace tracing
}  // namespace analysis
}  // namespace score
