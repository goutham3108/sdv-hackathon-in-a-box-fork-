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
#ifndef SCORE_LIB_SAFE_MATH_DETAILS_ABSOLUTE_ABSOLUTE_H
#define SCORE_LIB_SAFE_MATH_DETAILS_ABSOLUTE_ABSOLUTE_H

#include "score/language/safecpp/safe_math/details/comparison/comparison.h"
#include "score/language/safecpp/safe_math/details/type_traits/type_traits.h"

#include <score/math.hpp>

#include <limits>
#include <type_traits>

namespace score::safe_math
{

template <class T, typename std::enable_if_t<is_unsigned_integral<T>::value, bool> = true>
auto Abs(T value) noexcept -> T
{
    return value;
}

template <class Signed, typename std::enable_if_t<is_signed_integral<Signed>::value, bool> = true>
std::make_unsigned_t<Signed> Abs(Signed value) noexcept
{
    using Unsigned = std::make_unsigned_t<Signed>;
    if (CmpEqual(value, std::numeric_limits<Signed>::lowest()))
    {
        // Assumption is that the underlying implementation is using the 2's complement
        static_assert(Signed{-1} == static_cast<Signed>(static_cast<Unsigned>(~(Unsigned{0U}))),
                      "We expect 2's complement representation.");

        return static_cast<Unsigned>(std::numeric_limits<Signed>::max()) + 1U;
    }
    if (CmpLess(value, 0))
    {
        const auto inverse = static_cast<Signed>(-value);
        return static_cast<Unsigned>(inverse);
    }
    else
    {
        return static_cast<Unsigned>(value);
    }
}

template <class FT, typename std::enable_if_t<std::is_floating_point<FT>::value, bool> = true>
auto Abs(FT value) -> FT
{
    return static_cast<FT>(score::cpp::fabs(value));
}

}  // namespace score::safe_math

#endif  // SCORE_LIB_SAFE_MATH_DETAILS_ABSOLUTE_ABSOLUTE_H
