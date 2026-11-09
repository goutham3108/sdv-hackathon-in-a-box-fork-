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
#include "score/language/safecpp/scoped_function/details/modify_return_type.h"

#include <score/blank.hpp>

#include <gtest/gtest.h>

#include <cstdint>

namespace score::safecpp::details
{
namespace
{

class ModifyReturnTypeTest : public ::testing::Test
{
};

TEST_F(ModifyReturnTypeTest, TypeReturnsType)
{
    static_assert(std::is_same_v<ModifyReturnTypeT<std::int32_t>, std::int32_t>, "Types must equal");
}

TEST_F(ModifyReturnTypeTest, ConstTypeReturnsConstType)
{
    static_assert(std::is_same_v<ModifyReturnTypeT<const std::int32_t>, const std::int32_t>, "Types must equal");
}

TEST_F(ModifyReturnTypeTest, UniversalReferenceReturnsUniversalReference)
{
    static_assert(std::is_same_v<ModifyReturnTypeT<std::int32_t&&>, std::int32_t&&>, "Types must equal");
}

TEST_F(ModifyReturnTypeTest, ReferenceTypeReturnsWrappedReferenceType)
{
    static_assert(std::is_same_v<ModifyReturnTypeT<std::int32_t&>, std::reference_wrapper<std::int32_t>>,
                  "Types must equal");
}

TEST_F(ModifyReturnTypeTest, ConstReferenceTypeReturnsWrappedConstReferenceType)
{
    static_assert(std::is_same_v<ModifyReturnTypeT<const std::int32_t&>, std::reference_wrapper<const std::int32_t>>,
                  "Types must equal");
}

TEST_F(ModifyReturnTypeTest, VoidReturnsAmpBlank)
{
    static_assert(std::is_same_v<ModifyReturnTypeT<void>, score::cpp::blank>, "Types must equal");
}

}  // namespace
}  // namespace score::safecpp::details
