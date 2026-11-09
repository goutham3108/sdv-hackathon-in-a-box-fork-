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
#ifndef SCORE_LIB_RESULT_ERROR_CODE_H
#define SCORE_LIB_RESULT_ERROR_CODE_H

#include <cstdint>

namespace score
{
namespace result
{

/// \brief Every user-defined error code shall inherit from here
///
/// \details This way we can ensure that the user-defined error codes can be stored in our globally defined Error class.
using ErrorCode = std::int32_t;

}  // namespace result
}  // namespace score
#endif  // SCORE_LIB_RESULT_ERROR_CODE_H
