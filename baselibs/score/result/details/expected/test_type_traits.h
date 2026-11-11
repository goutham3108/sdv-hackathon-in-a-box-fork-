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
#ifndef SCORE_LIB_EXPECTED_TEST_TYPE_TRAITS_H
#define SCORE_LIB_EXPECTED_TEST_TYPE_TRAITS_H

#include <type_traits>

namespace score::details
{

template <typename T, typename... Args>
struct is_implicitly_constructible_impl : std::false_type
{
};

template <typename T, typename... Args>
struct is_implicitly_constructible_impl<T,
                                        decltype(void(std::declval<void (*)(T)>()({std::declval<Args>()...}))),
                                        Args...> : std::true_type
{
};

template <typename T, typename... Args>
using is_implicitly_constructible = is_implicitly_constructible_impl<T, void, Args...>;

template <typename T, typename... Args>
constexpr auto is_implicitly_constructible_v = is_implicitly_constructible<T, Args...>::value;

template <typename T, typename... Args>
constexpr auto is_only_explicitly_constructible_v =
    std::is_constructible_v<T, Args...> && !is_implicitly_constructible_v<T, Args...>;

}  // namespace score::details

#endif  // SCORE_LIB_EXPECTED_TEST_TYPE_TRAITS_H
