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

#ifndef SCORE_MW_LOG_DETAIL_ADD_ARGUMENT_RESULT_H
#define SCORE_MW_LOG_DETAIL_ADD_ARGUMENT_RESULT_H

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

enum class AddArgumentResult : bool
{
    kNotAdded = false,
    kAdded = true,
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_ADD_ARGUMENT_RESULT_H
