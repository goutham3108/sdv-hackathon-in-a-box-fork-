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
#ifndef SCORE_LIB_CONCURRENCY_FUTURE_ERROR_H
#define SCORE_LIB_CONCURRENCY_FUTURE_ERROR_H

#include "score/result/error.h"

namespace score
{
namespace concurrency
{

/// Reflects https://en.cppreference.com/w/cpp/thread/future_errc
enum class Error : score::result::ErrorCode
{
    kUnknown,
    kPromiseBroken,
    kFutureAlreadyRetrieved,
    kPromiseAlreadySatisfied,
    kNoState,
    kStopRequested,
    kTimeout,
    kUnset,
};

class FutureErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override;
};

score::result::Error MakeError(const score::concurrency::Error code, const std::string_view user_message = "") noexcept;

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_ERROR_H
