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

struct WrongType
{
};

template <class T>
class IsCallableFromTest : public ::testing::Test
{
};

struct MutableThrowableHelper
{
    auto GetCorrectCallable() noexcept
    {
        return [this](TypeA) mutable -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    auto GetCallableWithWrongReturnType() noexcept
    {
        return [this](TypeA) mutable -> WrongType {
            score::cpp::ignore = this;
            return {};
        };
    }

    auto GetCallableWithWrongParameter() noexcept
    {
        return [this](WrongType) mutable -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA);
};

struct ConstThrowableHelper
{
    auto GetCorrectCallable() noexcept
    {
        return [this](TypeA) -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    auto GetCallableWithWrongReturnType() noexcept
    {
        return [this](TypeA) -> WrongType {
            score::cpp::ignore = this;
            return {};
        };
    }

    auto GetCallableWithWrongParameter() noexcept
    {
        return [this](WrongType) -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA) const;
};

struct MutableNoExceptHelper
{
    auto GetCorrectCallable() noexcept
    {
        return [this](TypeA) mutable noexcept -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    auto GetCallableWithWrongReturnType() noexcept
    {
        return [this](TypeA) mutable noexcept -> WrongType {
            score::cpp::ignore = this;
            return {};
        };
    }

    auto GetCallableWithWrongParameter() noexcept
    {
        return [this](WrongType) mutable noexcept -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA) noexcept;
};

struct ConstNoExceptHelper
{
    auto GetCorrectCallable() noexcept
    {
        return [this](TypeA) noexcept -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    auto GetCallableWithWrongReturnType() noexcept
    {
        return [this](TypeA) noexcept -> WrongType {
            score::cpp::ignore = this;
            return {};
        };
    }

    auto GetCallableWithWrongParameter() noexcept
    {
        return [this](WrongType) noexcept -> TypeB {
            score::cpp::ignore = this;
            return {};
        };
    }

    using ExpectedFunctionType = TypeB(TypeA) const noexcept;
};

using Types =
    ::testing::Types<MutableThrowableHelper, ConstThrowableHelper, MutableNoExceptHelper, ConstNoExceptHelper>;
TYPED_TEST_SUITE(IsCallableFromTest, Types, /* unused */);

TYPED_TEST(IsCallableFromTest, IsCallableIfReturnTypeArgumentsAndModifiersMatch)
{
    static_assert(
        IsCallableFrom<decltype(TypeParam{}.GetCorrectCallable()), typename TypeParam::ExpectedFunctionType>::value,
        "Must be callable");
}

TYPED_TEST(IsCallableFromTest, IsNotCallableIfReturnTypeWrong)
{
    static_assert(!IsCallableFrom<decltype(TypeParam{}.GetCallableWithWrongReturnType()),
                                  typename TypeParam::ExpectedFunctionType>::value,
                  "Must not be callable");
}

TYPED_TEST(IsCallableFromTest, IsNotCallableIfArgumentsWrong)
{
    static_assert(!IsCallableFrom<decltype(TypeParam{}.GetCallableWithWrongParameter()),
                                  typename TypeParam::ExpectedFunctionType>::value,
                  "Must not be callable");
}

}  // namespace
}  // namespace score::safecpp::details
