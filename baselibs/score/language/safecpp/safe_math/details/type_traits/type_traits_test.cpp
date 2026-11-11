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
#include "score/language/safecpp/safe_math/details/type_traits/type_traits.h"

#include <gtest/gtest.h>

namespace score::safe_math
{

class Dummy
{
};

TEST(TypeTraitsTest, IsSignedIntegral)
{
    static_assert(is_signed_integral<Dummy>::value == false, "");
    static_assert(is_signed_integral<std::uint8_t>::value == false, "");
    static_assert(is_signed_integral<std::int8_t>::value == true, "");
    static_assert(is_signed_integral<double>::value == false, "");
}

TEST(TypeTraitsTest, IsUnsignedIntegral)
{
    static_assert(is_unsigned_integral<Dummy>::value == false, "");
    static_assert(is_unsigned_integral<std::uint8_t>::value == true, "");
    static_assert(is_unsigned_integral<std::int8_t>::value == false, "");
    static_assert(is_unsigned_integral<double>::value == false, "");
}

TEST(TypeTraitsTest, BiggerType)
{
    static_assert(std::is_same<bigger_type_t<std::uint16_t, std::uint16_t>, std::uint16_t>::value == true, "");
    static_assert(std::is_same<bigger_type_t<std::uint16_t, std::uint32_t>, std::uint32_t>::value == true, "");
    static_assert(std::is_same<bigger_type_t<std::uint32_t, std::uint16_t>, std::uint32_t>::value == true, "");
    static_assert(std::is_same<bigger_type_t<std::int16_t, std::int16_t>, std::int16_t>::value == true, "");
    static_assert(std::is_same<bigger_type_t<std::int16_t, std::int32_t>, std::int32_t>::value == true, "");
    static_assert(std::is_same<bigger_type_t<std::int32_t, std::int16_t>, std::int32_t>::value == true, "");
    static_assert(std::is_same<bigger_type_t<float, double>, double>::value == true, "");
    static_assert(std::is_same<bigger_type_t<double, float>, double>::value == true, "");
    static_assert(std::is_same<bigger_type_t<long double, float>, long double>::value == true, "");
    static_assert(std::is_same<bigger_type_t<long double, double>, long double>::value == true, "");
}

TEST(TypeTraitsTest, PreferFirstType)
{
    static_assert(std::is_same<prefer_first_type_t<std::int32_t, std::uint32_t>, std::int32_t>::value == true, "");
    static_assert(std::is_same<prefer_first_type_t<void, std::uint32_t>, std::uint32_t>::value == true, "");
}

}  // namespace score::safe_math
