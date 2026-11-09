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

#ifndef SCORE_LIB_JSON_INTERNAL_MODEL_LOSSLESS_CAST_H
#define SCORE_LIB_JSON_INTERNAL_MODEL_LOSSLESS_CAST_H

#include "score/result/result.h"
#include <type_traits>

namespace score
{
namespace json
{

///
/// @brief Casts the input to the output type iff the value can be represented identically in the output type.
/// Overload for the case Input and Output type are same.
///
template <typename Output,
          typename Input,
          typename = std::enable_if_t<std::is_arithmetic<Input>::value, bool>,
          typename = std::enable_if_t<std::is_arithmetic<Output>::value, bool>,
          typename = std::enable_if_t<std::is_same<Input, Output>::value, bool>>
// Rationale: False positive. This is not a redeclaration but a template specializations.
// coverity[autosar_cpp14_m3_2_3_violation]
score::Result<Output> TryLosslessCast(Input input);

///
/// @brief Casts the input to the output type iff the value can be represented identically in the output type.
/// Overload for the case Input is an integer type.
///
template <typename Output,
          typename Input,
          typename = std::enable_if_t<std::is_arithmetic<Input>::value, bool>,
          typename = std::enable_if_t<std::is_arithmetic<Output>::value, bool>,
          typename = std::enable_if_t<!std::is_same<Input, Output>::value, bool>,
          typename = std::enable_if_t<std::is_integral<Input>::value, bool>,
          typename = std::enable_if_t<!std::is_same<Output, bool>::value, bool>>
// Rationale: False positive. This is not a redeclaration but a template specializations.
// coverity[autosar_cpp14_m3_2_3_violation]
score::Result<Output> TryLosslessCast(Input input);

///
/// @brief Casts the input to the output type iff the value can be represented identically in the output type.
/// Overload for the case Input is a floating point type.
///
template <typename Output,
          typename Input,
          typename = std::enable_if_t<std::is_arithmetic<Input>::value, bool>,
          typename = std::enable_if_t<std::is_arithmetic<Output>::value, bool>,
          typename = std::enable_if_t<!std::is_same<Input, Output>::value, bool>,
          typename = std::enable_if_t<!std::is_integral<Input>::value, bool>,
          typename = std::enable_if_t<!std::is_integral<Input>::value, bool>,
          typename = std::enable_if_t<!std::is_same<Output, bool>::value, bool>>
// Rationale: False positive. This is not a redeclaration but a template specializations.
// coverity[autosar_cpp14_m3_2_3_violation]
score::Result<Output> TryLosslessCast(Input input);

///
/// @brief Casts the input to the output type iff the value can be represented identically in the output type.
/// Overload for the case Output is a bool.
///
template <typename Output,
          typename Input,
          typename = std::enable_if_t<std::is_arithmetic<Input>::value, bool>,
          typename = std::enable_if_t<std::is_arithmetic<Output>::value, bool>,
          typename = std::enable_if_t<!std::is_same<Input, Output>::value, bool>,
          typename = std::enable_if_t<std::is_same<Output, bool>::value, bool>,
          bool = false,
          bool = false,
          bool = false>
// Rationale: False positive. This is not a redeclaration but a template specializations.
// coverity[autosar_cpp14_m3_2_3_violation]
score::Result<Output> TryLosslessCast(Input input);

// Explicit instantiation declaration for TryLosslessCast() to meet MISRA requirements.
extern template score::Result<std::int8_t> score::json::TryLosslessCast(std::int8_t);
extern template score::Result<std::int8_t> score::json::TryLosslessCast(std::int16_t);
extern template score::Result<std::int8_t> score::json::TryLosslessCast(std::int32_t);
extern template score::Result<std::int8_t> score::json::TryLosslessCast(std::int64_t);
extern template score::Result<std::int8_t> score::json::TryLosslessCast(std::uint8_t);
extern template score::Result<std::int8_t> score::json::TryLosslessCast(std::uint16_t);
extern template score::Result<std::int8_t> score::json::TryLosslessCast(std::uint32_t);
extern template score::Result<std::int8_t> score::json::TryLosslessCast(std::uint64_t);
extern template score::Result<std::int8_t> score::json::TryLosslessCast(float);
extern template score::Result<std::int8_t> score::json::TryLosslessCast(double);

}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_MODEL_LOSSLESS_CAST_H
