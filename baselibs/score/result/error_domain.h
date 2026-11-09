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
#ifndef SCORE_LIB_RESULT_ERROR_DOMAIN_H
#define SCORE_LIB_RESULT_ERROR_DOMAIN_H

#include "score/result/error_code.h"

#include <string_view>

namespace score
{
namespace result
{

/// \brief Interface for user-defined error domains. The domains are used to translate user-defined error codes into
/// human readable messages.
///
/// \details For examples on how you can provide your custom ErrorDomain, please see respective error_test.cpp Unit
/// Tests.
class ErrorDomain
{
  public:
    constexpr ErrorDomain() = default;

    /// \brief Translates an error code into a human readable message
    ///
    /// \param code The error code to translates
    /// \return returns a string view to the human readable message
    virtual std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept = 0;

  protected:
    constexpr ErrorDomain(const ErrorDomain&) = default;
    constexpr ErrorDomain(ErrorDomain&&) noexcept = default;
    ErrorDomain& operator=(const ErrorDomain&) = default;
    ErrorDomain& operator=(ErrorDomain&&) noexcept = default;
    // no polymorphic destruction needed, needs to be non virtual to be trivial type
    ~ErrorDomain() = default;
};

}  // namespace result
}  // namespace score

#endif  // SCORE_LIB_RESULT_ERROR_DOMAIN_H
