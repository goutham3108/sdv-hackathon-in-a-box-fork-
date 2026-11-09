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
#include "score/language/safecpp/scoped_function/details/is_callable_from.h"

#include <score/utility.hpp>

#include <gtest/gtest.h>

namespace score::safecpp::details
{
namespace
{

struct TypeA
{
};

struct TypeB
{
};

template <class T>
class IsCallableFromWithWrongModifiersTest : public ::testing::Test
{
};

struct ConstThrowableHelperWithWrongModifiers
{

    auto GetCallableWithWrongModifier() noexcept
    {
        return [this](TypeA) mutable -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA) const;
};

struct MutableNoExceptHelperWithWrongModifiers
{

    auto GetCallableWithWrongModifier() noexcept
    {
        return [this](TypeA) mutable -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA) noexcept;
};

struct ConstNoExceptHelperWithWrongModifiers
{

    auto GetCallableWithWrongModifier() noexcept
    {
        return [this](TypeA) -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA) const noexcept;
};

using TypesWithWrongModifiers = ::testing::Types<ConstThrowableHelperWithWrongModifiers,
                                                 MutableNoExceptHelperWithWrongModifiers,
                                                 ConstNoExceptHelperWithWrongModifiers>;
TYPED_TEST_SUITE(IsCallableFromWithWrongModifiersTest, TypesWithWrongModifiers, /* unused */);

TYPED_TEST(IsCallableFromWithWrongModifiersTest, IsNotCallableIfModifiersWrong)
{
    static_assert(!IsCallableFromV<decltype(TypeParam{}.GetCallableWithWrongModifier()),
                                   typename TypeParam::ExpectedFunctionType>,
                  "Must not be callable");
}

}  // namespace
}  // namespace score::safecpp::details
