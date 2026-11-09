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
#ifndef SCORE_LIB_SAFE_MATH_RETURN_MODE_H
#define SCORE_LIB_SAFE_MATH_RETURN_MODE_H

#include "score/result/result.h"

#include <cstdlib>
#include <type_traits>

namespace score::safe_math
{

/// Defines the error handling behavior for safe math operations.
/// Controls whether operations return Result types for error handling or abort on errors.
enum class ReturnMode
{
    kReturnResultOnError,  // Returns score::Result<T>
    kAbortOnError          // Performance mode, returns T directly or aborts program execution on error
};

// Default return mode for safe math operations
constexpr inline auto kDefaultReturnMode = ReturnMode::kReturnResultOnError;

/// Central type alias for return types based on ReturnMode
/// In kReturnResultOnError mode: returns score::Result<R>
/// In kAbortOnError mode: returns R directly
template <typename R, ReturnMode return_mode>
using ModeBasedReturnType = std::conditional_t<return_mode == ReturnMode::kReturnResultOnError, score::Result<R>, R>;

/// @brief utility to be used within `if constexpr`s to indicate an unhandled `ReturnMode` value
template <ReturnMode return_mode>
[[noreturn]] void EmitCompilerDiagnosticForUnhandledEnumerator() noexcept
{
    // `static_assert` is (by intention) always false to emit compiler diagnostic
    static_assert(std::is_void_v<ReturnMode>, "unhandled enumerator value for `ReturnMode`");
    std::abort();  // dummy statement to avoid -Wreturn-type diagnostic for missing return
}

/// \brief Handles error cases uniformly across both return modes.
/// \details This helper function encapsulates the error handling pattern used throughout safe_math operations.
///          It provides a single point of abstraction for converting errors to the appropriate return type
///          based on the active ReturnMode.
/// \tparam R The successful return type (not Result<R>, just R)
/// \tparam return_mode The ReturnMode to use for error handling (kReturnResultOnError or kAbortOnError)
/// \tparam ErrorType The type of the error (typically score::Unexpected or auto-deduced)
/// \param error The error to handle (only used in kReturnResultOnError mode)
/// \return In kReturnResultOnError mode: returns Result<R> containing the error.
///         In kAbortOnError mode: calls std::abort() and never returns.
/// \note This function is marked [[maybe_unused]] for error parameter because in kAbortOnError mode,
///       the parameter is not used (std::abort is called immediately).
/// \example Usage in a function returning ModeBasedReturnType<int, return_mode>:
///          return HandleError<int, return_mode>(score::MakeUnexpected(ErrorCode::kOverflow));
template <typename R, ReturnMode return_mode, typename ErrorType>
ModeBasedReturnType<R, return_mode> HandleError([[maybe_unused]] ErrorType error)
{
    if constexpr (return_mode == ReturnMode::kReturnResultOnError)
    {
        return error;
    }
    else if constexpr (return_mode == ReturnMode::kAbortOnError)
    {
        std::abort();
    }
    else
    {
        EmitCompilerDiagnosticForUnhandledEnumerator<return_mode>();
    }
}

/// \brief Applies an action based on the return mode, handling Result unwrapping automatically.
/// \details This helper function encapsulates the pattern of applying an action/transformation
///          that needs to work with both Result<T> (kReturnResultOnError) and T (kAbortOnError).
///          It automatically uses and_then() for Result types or calls the action directly.
/// \tparam return_mode The ReturnMode controlling the behavior
/// \tparam T The value type (not Result<T>, just T)
/// \tparam Action A callable that takes T and returns ModeBasedReturnType<R, return_mode>
/// \param action The action to apply to the value
/// \param value The value to process (either Result<T> or T depending on return_mode)
/// \return The result of applying the action (ModeBasedReturnType<R, return_mode>)
/// \example Usage with functor:
///          return PerformActionBasedOnReturnMode<return_mode, int>(MyChecker{args}, some_value);
template <ReturnMode return_mode, typename T, typename Action>
auto PerformActionBasedOnReturnMode(Action action, const ModeBasedReturnType<T, return_mode>& value) noexcept
{
    if constexpr (return_mode == ReturnMode::kReturnResultOnError)
    {
        return value.and_then(action);
    }
    else if constexpr (return_mode == ReturnMode::kAbortOnError)
    {
        return action(value);
    }
    else
    {
        EmitCompilerDiagnosticForUnhandledEnumerator<return_mode>();
    }
}

}  // namespace score::safe_math

#endif  // SCORE_LIB_SAFE_MATH_RETURN_MODE_H
