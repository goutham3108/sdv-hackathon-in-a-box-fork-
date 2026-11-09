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
#ifndef SCORE_LIB_SAFE_MATH_DETAILS_TEST_TYPE_COLLECTION_H
#define SCORE_LIB_SAFE_MATH_DETAILS_TEST_TYPE_COLLECTION_H

#include <gtest/gtest.h>

#include <cstdint>
#include <utility>

namespace score::safe_math
{

using UnsignedTypes = ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

using IntegerTypes = ::testing::Types<std::uint8_t,
                                      std::uint16_t,
                                      std::uint32_t,
                                      std::uint64_t,
                                      std::int8_t,
                                      std::int16_t,
                                      std::int32_t,
                                      std::int64_t>;

// We use same valid type pairs that are likely to cause issues if there are any. This is intentionally not the full
// list of supported types to keep build times below several minutes.
using UnsignedTypePairs = ::testing::Types<std::pair<unsigned int, unsigned int>,
                                           std::pair<std::uint8_t, std::uint8_t>,
                                           std::pair<std::uint8_t, std::uint64_t>,
                                           std::pair<std::uint64_t, std::uint8_t>>;

using SignedTypePairs = ::testing::Types<std::pair<int, int>,
                                         std::pair<std::int8_t, std::int8_t>,
                                         std::pair<std::int8_t, std::int64_t>,
                                         std::pair<std::int64_t, std::int8_t>>;

using SignedUnsignedTypePairs = ::testing::Types<std::pair<int, unsigned int>,
                                                 std::pair<std::int8_t, std::uint8_t>,
                                                 std::pair<std::int8_t, std::uint64_t>,
                                                 std::pair<std::int64_t, std::uint8_t>>;

using UnsignedSignedTypePairs = ::testing::Types<std::pair<unsigned int, int>,
                                                 std::pair<std::uint8_t, std::int8_t>,
                                                 std::pair<std::uint8_t, std::int64_t>,
                                                 std::pair<std::uint64_t, std::int64_t>>;

using FloatingPointFloatingPointTypePairs = ::testing::Types<std::pair<float, double>, std::pair<double, float>>;

}  // namespace score::safe_math

#endif  // SCORE_LIB_SAFE_MATH_DETAILS_TEST_TYPE_COLLECTION_H
