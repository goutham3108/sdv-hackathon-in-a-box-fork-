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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_CALL_AND_RETURN_MODIFIED_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_CALL_AND_RETURN_MODIFIED_H

#include "score/language/safecpp/scoped_function/details/modify_return_type.h"

#include <score/blank.hpp>

#include <type_traits>
#include <utility>

namespace score::safecpp::details
{
template <class... Args,
          class Callable,
          class ReturnType = typename score::cpp::invoke_result_t<Callable, Args...>,
          std::enable_if_t<!std::is_void_v<ReturnType>, bool> = true>
[[nodiscard]] auto CallAndReturnModified(Callable& callable, Args&&... args)
{
    // Suppress "AUTOSAR C++14 A5-2-2" rule finding.
    // This rule states: "Traditional C-style casts shall not be used."
    // Rationale: We want to construct the target type with the result of the callable. This not a cast.
    // coverity[autosar_cpp14_a5_2_2_violation]
    return ModifyReturnTypeT<ReturnType>{callable(std::forward<Args>(args)...)};
}

template <class... Args,
          class Callable,
          class ReturnType = typename score::cpp::invoke_result_t<Callable, Args...>,
          std::enable_if_t<std::is_void_v<ReturnType>, bool> = true>
[[nodiscard]] score::cpp::blank CallAndReturnModified(Callable& callable, Args&&... args)
{
    callable(std::forward<Args>(args)...);
    return score::cpp::blank{};
}

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_CALL_AND_RETURN_MODIFIED_H
