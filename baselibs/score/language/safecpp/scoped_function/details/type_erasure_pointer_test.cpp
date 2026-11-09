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
#include "score/language/safecpp/scoped_function/details/type_erasure_pointer.h"
#include "score/language/safecpp/scoped_function/details/instrumented_memory_resource.h"

#include <score/memory.hpp>

#include <gtest/gtest.h>

namespace score::safecpp::details
{
namespace
{

class SomeImplementation
{
  public:
    virtual ~SomeImplementation() = default;
};

class TypeErasurePointerTest : public testing::Test
{
};

TEST_F(TypeErasurePointerTest, MakeTypeErasurePointerUsesProvidedAllocator)
{
    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<SomeImplementation> polymorphic_allocator{&instrumented_memory_resource};
    const auto pointer = MakeTypeErasurePointer<SomeImplementation>(polymorphic_allocator);
    EXPECT_NE(pointer, nullptr);
    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);
}

TEST_F(TypeErasurePointerTest, CannotCopyConstruct)
{
    static_assert(!std::is_copy_constructible_v<TypeErasurePointer<SomeImplementation>>);
}

TEST_F(TypeErasurePointerTest, CannotCopyAssign)
{
    static_assert(!std::is_copy_assignable_v<TypeErasurePointer<SomeImplementation>>);
}

TEST_F(TypeErasurePointerTest, CanMoveConstructWithoutReallocation)
{
    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<SomeImplementation> polymorphic_allocator{&instrumented_memory_resource};
    auto pointer = MakeTypeErasurePointer<SomeImplementation>(polymorphic_allocator);
    EXPECT_NE(pointer, nullptr);

    TypeErasurePointer<SomeImplementation> moved_pointer{std::move(pointer)};

    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);
}

TEST_F(TypeErasurePointerTest, CanMoveAssignWithoutReallocation)
{
    InstrumentedMemoryResource instrumented_memory_resource{};
    score::cpp::pmr::polymorphic_allocator<SomeImplementation> polymorphic_allocator{&instrumented_memory_resource};
    auto pointer = MakeTypeErasurePointer<SomeImplementation>(polymorphic_allocator);
    EXPECT_NE(pointer, nullptr);

    TypeErasurePointer<SomeImplementation> moved_pointer{nullptr, TypeErasurePointerDeleter{polymorphic_allocator}};
    moved_pointer = std::move(pointer);

    EXPECT_EQ(instrumented_memory_resource.GetNumberOfAllocations(), 1);
}

TEST_F(TypeErasurePointerTest, MoveAssignmentDoesNotAdoptMemoryResource)
{
    InstrumentedMemoryResource allocation_instrumented_memory_resource{};
    InstrumentedMemoryResource deallocation_instrumented_memory_resource{};

    {
        score::cpp::pmr::polymorphic_allocator<SomeImplementation> polymorphic_allocator{
            &allocation_instrumented_memory_resource};
        auto pointer = MakeTypeErasurePointer<SomeImplementation>(polymorphic_allocator);
        EXPECT_NE(pointer, nullptr);

        score::cpp::pmr::polymorphic_allocator<SomeImplementation> unused_polymorphic_allocator{
            &deallocation_instrumented_memory_resource};

        TypeErasurePointer<SomeImplementation> moved_pointer{nullptr,
                                                             TypeErasurePointerDeleter{unused_polymorphic_allocator}};
        moved_pointer = std::move(pointer);
    }

    constexpr ssize_t only_one_allocation{1};
    EXPECT_EQ(allocation_instrumented_memory_resource.GetNumberOfAllocations(), only_one_allocation);

    constexpr ssize_t only_one_deallocation{-1};
    EXPECT_EQ(deallocation_instrumented_memory_resource.GetNumberOfAllocations(), only_one_deallocation);
}

}  // namespace
}  // namespace score::safecpp::details
