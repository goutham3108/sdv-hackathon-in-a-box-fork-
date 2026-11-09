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
#ifndef SCORE_LIB_EXPECTED_TEST_TYPES_H
#define SCORE_LIB_EXPECTED_TEST_TYPES_H

#include <cstdint>
#include <utility>

namespace score::details
{

class ValueType
{
};

class CopyableType
{
  public:
    explicit CopyableType(std::int32_t value) noexcept : value_{value} {}

    bool operator==(const CopyableType& other) const noexcept
    {
        return value_ == other.value_;
    }

    std::int32_t value_;
};

class CompatibleCopyableType
{
  public:
    explicit CompatibleCopyableType(CopyableType t) noexcept : inner_{t} {}

    CopyableType inner_;
};

class ThrowMoveOnlyType
{
  public:
    explicit ThrowMoveOnlyType(std::int32_t value) noexcept : value_{value} {}

    ThrowMoveOnlyType(const ThrowMoveOnlyType&) = delete;
    ThrowMoveOnlyType(ThrowMoveOnlyType&& other) : value_{std::move(other.value_)} {};
    ThrowMoveOnlyType& operator=(const ThrowMoveOnlyType&) = delete;
    ThrowMoveOnlyType& operator=(ThrowMoveOnlyType&&) = default;
    bool operator==(const ThrowMoveOnlyType& other) const
    {
        return value_ == other.value_;
    }

    std::int32_t value_;
};

class NothrowMoveOnlyType
{
  public:
    explicit NothrowMoveOnlyType(std::int32_t value) noexcept : value_{value} {}

    NothrowMoveOnlyType(const NothrowMoveOnlyType&) = delete;
    NothrowMoveOnlyType(NothrowMoveOnlyType&&) noexcept = default;
    NothrowMoveOnlyType& operator=(const NothrowMoveOnlyType&) = delete;
    NothrowMoveOnlyType& operator=(NothrowMoveOnlyType&&) noexcept = default;
    bool operator==(const NothrowMoveOnlyType& other) const noexcept
    {
        return value_ == other.value_;
    }

    std::int32_t value_;
};

class CompatibleNothrowMoveOnlyType
{
  public:
    explicit CompatibleNothrowMoveOnlyType(NothrowMoveOnlyType t) noexcept : inner_{std::move(t)} {}

    NothrowMoveOnlyType inner_;
};

class ArgumentType
{
  public:
    ArgumentType(CopyableType copyable, NothrowMoveOnlyType moveonly) noexcept
        : copyable_{copyable}, moveonly_{std::move(moveonly)}
    {
    }

    CopyableType copyable_;
    NothrowMoveOnlyType moveonly_;
};

class ArgumentInitializerListType
{
  public:
    ArgumentInitializerListType(std::initializer_list<CopyableType>& copyable, NothrowMoveOnlyType moveonly) noexcept
        : copyable_{*copyable.begin()}, moveonly_{std::move(moveonly)}
    {
    }

    CopyableType copyable_;
    NothrowMoveOnlyType moveonly_;
};

class UnmovableType
{
  public:
    explicit UnmovableType(std::int32_t value) noexcept : value_{value} {}

    UnmovableType(const UnmovableType&) = delete;
    UnmovableType(UnmovableType&&) = delete;
    UnmovableType& operator=(const UnmovableType&) = delete;
    UnmovableType& operator=(UnmovableType&&) = delete;

    std::int32_t value_;
};

class ErrorType
{
};

}  // namespace score::details

#endif  // SCORE_LIB_EXPECTED_TEST_TYPES_H
