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

#ifndef SCORE_MW_LOG_DETAIL_DLT_ARGUMENT_COUNTER_H
#define SCORE_MW_LOG_DETAIL_DLT_ARGUMENT_COUNTER_H

#include "score/callback.hpp"
#include "score/mw/log/detail/add_argument_result.h"

#include <cstdint>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class DltArgumentCounter
{
  public:
    using AddArgumentCallback = score::cpp::callback<AddArgumentResult(void), 64UL>;

    explicit DltArgumentCounter(std::uint8_t&) noexcept;
    AddArgumentResult TryAddArgument(AddArgumentCallback) noexcept;

  private:
    std::uint8_t& counter_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_DLT_ARGUMENT_COUNTER_H
