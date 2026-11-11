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
#include "score/mw/com/impl/bindings/lola/methods/method_data.h"

#include "score/mw/com/impl/bindings/lola/methods/type_erased_call_queue.h"
#include "score/mw/com/impl/configuration/lola_method_id.h"

#include "score/memory/shared/fake/my_bounded_memory_resource.h"

#include <score/utility.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <utility>

namespace score::mw::com::impl::lola
{
namespace
{

using namespace ::testing;

const std::size_t kNumberOfElements{5U};

class MethodDataFixture : public ::testing::Test
{
  public:
    memory::shared::test::MyBoundedMemoryResource fake_memory_resource_{2000U};
};

TEST_F(MethodDataFixture, ConstructingMethodDataWithSizeZeroWillNotAllocate)
{
    // When Constructing a MethodData object with size zero
    MethodData unit{0U, fake_memory_resource_};

    // Then no memory should have been allocated
    EXPECT_EQ(fake_memory_resource_.GetUserAllocatedBytes(), 0U);
}

TEST_F(MethodDataFixture, ConstructingMethodDataWithSizeNWillAllocateNElementsInArray)
{
    // When Constructing a MethodData object with size 5
    MethodData unit{kNumberOfElements, fake_memory_resource_};

    // Then memory should have been allocated for each element
    const auto size_of_method_call_queue_element = sizeof(std::pair<LolaMethodId, TypeErasedCallQueue>);
    const auto total_allocated_bytes = size_of_method_call_queue_element * kNumberOfElements;
    EXPECT_EQ(fake_memory_resource_.GetUserAllocatedBytes(), total_allocated_bytes);
}

TEST_F(MethodDataFixture, ConstructingMethodDataInMemoryResourceWillAllocateArrayAndAllElements)
{
    // When Constructing a MethodData object with size 5 in the memory resource provided to MethodData
    score::cpp::ignore = fake_memory_resource_.construct<MethodData>(kNumberOfElements, fake_memory_resource_);

    // Then memory should have been allocated for the MethodData object and each element
    const auto size_of_method_data = sizeof(MethodData);
    const auto size_of_method_call_queue_element = sizeof(std::pair<LolaMethodId, TypeErasedCallQueue>);
    const auto total_allocated_bytes = size_of_method_call_queue_element * kNumberOfElements + size_of_method_data;
    EXPECT_EQ(fake_memory_resource_.GetUserAllocatedBytes(), total_allocated_bytes);
}

}  // namespace
}  // namespace score::mw::com::impl::lola
