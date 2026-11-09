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
#ifndef SCORE_MW_LOG_DETAIL_INTEGER_REPRESENTATION_H
#define SCORE_MW_LOG_DETAIL_INTEGER_REPRESENTATION_H

#include <cstdint>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief The output representation when formatting the number to a string on the receiver side, e.g. in DltViewer.
/// \note Enum values shall correspond to requirement PRS_Dlt_00783.
enum class IntegerRepresentation : std::uint8_t
{
    kDecimal = 0x00,
    kOctal = 0x01,
    kHex = 0x02,
    kBinary = 0x03
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_INTEGER_REPRESENTATION_H
