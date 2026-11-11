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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_MODIFY_RETURN_TYPE_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_MODIFY_RETURN_TYPE_H

#include <score/blank.hpp>
#include <score/optional.hpp>

#include <functional>

namespace score::safecpp::details
{

template <class T>
struct ModifyReturnType
{
    // Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
    // Rationale: There is not decltype or typeid.
    // coverity[autosar_cpp14_a5_1_7_violation]
    using Type = T;
};

template <class T>
struct ModifyReturnType<T&>
{
    using Type = std::reference_wrapper<T>;
};

template <>
struct ModifyReturnType<void>
{
    using Type = score::cpp::blank;
};

template <class T>
// Suppress "AUTOSAR C++14 A5-1-7" rule finding: "A lambda shall not be an operand to decltype or typeid.".
// Rationale: There is not decltype or typeid.
// coverity[autosar_cpp14_a5_1_7_violation]
using ModifyReturnTypeT = typename ModifyReturnType<T>::Type;

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_MODIFY_RETURN_TYPE_H
