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
#ifndef SCORE_LIB_SAFE_MATH_DETAILS_TYPE_TRAITS_TYPE_TRAITS_H
#define SCORE_LIB_SAFE_MATH_DETAILS_TYPE_TRAITS_TYPE_TRAITS_H

#include <limits>
#include <type_traits>

namespace score::safe_math
{
template <class T, class EnableIf = void>
struct is_signed_integral : public std::false_type
{
};

template <class T>
struct is_signed_integral<T, typename std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value>>
    : public std::true_type
{
};

template <class T, class EnableIf = void>
struct is_unsigned_integral : public std::false_type
{
};

template <class T>
struct is_unsigned_integral<T, typename std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value>>
    : public std::true_type
{
};

template <class T1,
          class T2,
          typename std::enable_if_t<((std::is_integral<T1>::value && std::is_integral<T2>::value) ||
                                     (std::is_floating_point<T1>::value && std::is_floating_point<T2>::value)) &&
                                        std::is_signed<T1>::value == std::is_signed<T2>::value,
                                    bool> = true>
using bigger_type_t = std::conditional_t<(std::numeric_limits<T1>::digits >= std::numeric_limits<T2>::digits), T1, T2>;

template <class T1, class T2>
using prefer_first_type_t = std::conditional_t<!std::is_void<T1>::value, T1, T2>;

}  // namespace score::safe_math

#endif  // SCORE_LIB_SAFE_MATH_DETAILS_TYPE_TRAITS_TYPE_TRAITS_H
