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
class IsCallableFromWithCompatibleModifiersTest : public ::testing::Test
{
};

struct MutableHelperWithCompatibleConstModifiers
{

    auto GetCallableWithCompatibleModifier() noexcept
    {
        return [this](TypeA) -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA);
};

struct MutableHelperWithCompatibleNoExceptModifiers
{

    auto GetCallableWithCompatibleModifier() noexcept
    {
        return [this](TypeA) noexcept -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA);
};

struct ConstHelperWithCompatibleNoExceptModifiers
{

    auto GetCallableWithCompatibleModifier() noexcept
    {
        return [this](TypeA) noexcept -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA) const;
};

struct NoexceptHelperWithCompatibleConstModifiers
{

    auto GetCallableWithCompatibleModifier() noexcept
    {
        return [this](TypeA) noexcept -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA) noexcept;
};

using TypesWithCompatibleModifiers = ::testing::Types<MutableHelperWithCompatibleConstModifiers,
                                                      MutableHelperWithCompatibleNoExceptModifiers,
                                                      ConstHelperWithCompatibleNoExceptModifiers,
                                                      NoexceptHelperWithCompatibleConstModifiers>;
TYPED_TEST_SUITE(IsCallableFromWithCompatibleModifiersTest, TypesWithCompatibleModifiers, /* unused */);

TYPED_TEST(IsCallableFromWithCompatibleModifiersTest, IsCallableIfModifiersCompatible)
{
    static_assert(IsCallableFromV<decltype(TypeParam{}.GetCallableWithCompatibleModifier()),
                                  typename TypeParam::ExpectedFunctionType>,
                  "Must be callable");
}

}  // namespace
}  // namespace score::safecpp::details
