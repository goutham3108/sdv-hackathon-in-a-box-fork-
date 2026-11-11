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

#include "score/json/internal/parser/nlohmann/json_builder.h"

#include "score/json/internal/model/any.h"
#include "score/json/internal/model/error.h"
#include "score/json/internal/model/object.h"
#include "score/result/result.h"

#include "nlohmann/json.hpp"

#include <atomic>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>

namespace
{

using std::numeric_limits;

template <typename T, typename U>
bool CanTypeFitValue(const U value)
{
    const intmax_t lower_T = intmax_t(numeric_limits<T>::min());
    const intmax_t lower_U = intmax_t(numeric_limits<U>::min());
    const uintmax_t upper_T = uintmax_t(numeric_limits<T>::max());
    const uintmax_t upper_U = uintmax_t(numeric_limits<U>::max());
    return !((lower_T > lower_U && value < static_cast<U>(lower_T)) ||
             (upper_T < upper_U && value > static_cast<U>(upper_T)));
}
}  // namespace

auto score::json::JsonBuilder::HandleEvent(nlohmann::json::parse_event_t event, nlohmann::json& parsed) -> bool
{
    switch (event)
    {
        case nlohmann::json::parse_event_t::object_start:
            return StartContainer(score::json::Object{});

        case nlohmann::json::parse_event_t::object_end:
            return EndContainer();

        case nlohmann::json::parse_event_t::array_start:
            return StartContainer(score::json::List{});

        case nlohmann::json::parse_event_t::array_end:
            return EndContainer();

        case nlohmann::json::parse_event_t::key:
            last_key_ = parsed;
            break;

        case nlohmann::json::parse_event_t::value:
            return StoreValue(parsed);
            break;

        default:
            std::cerr << "Unknown event type\n";
            return false;
    }

    return true;
}

auto score::json::JsonBuilder::EndContainer() -> bool
{
    hierarchy_.pop();
    return true;
}

auto score::json::JsonBuilder::StoreValue(nlohmann::json& parsed) -> bool
{

    bool ret;
    if (parsed.is_boolean())
    {
        ret = Store(std::forward<bool>(parsed)).has_value();
        return ret;
    }
    else if (parsed.is_string())
    {
        ret = Store(std::forward<std::string>(parsed)).has_value();
        return ret;
    }

    else if (parsed.is_null())
    {
        return Store(score::json::Null{}).has_value();
    }

    // The parser shall try first the unsigned types from smallest to largest.
    // Then it shall try the signed types from smallest to largest.
    // Finally it will try double.
    // This way the model can assume that the number type is the "smallest possible type" except from floating-point
    // numbers, they shall be always presented as a 'double'
    else if (parsed.is_number_unsigned())
    {
        auto number{static_cast<std::uint64_t>(parsed)};
        if (CanTypeFitValue<std::uint8_t>(number))
        {
            return Store(score::json::Number{static_cast<std::uint8_t>(parsed)}).has_value();
        }

        if (CanTypeFitValue<std::uint16_t>(number))
        {
            return Store(score::json::Number{static_cast<std::uint16_t>(parsed)}).has_value();
        }

        if (CanTypeFitValue<std::uint32_t>(number))
        {
            return Store(score::json::Number{static_cast<std::uint32_t>(parsed)}).has_value();
        }

        if (CanTypeFitValue<std::uint64_t>(number))
        {
            return Store(score::json::Number{static_cast<std::uint64_t>(parsed)}).has_value();
        }
    }

    else if (parsed.is_number_integer())
    {
        auto number{static_cast<std::int64_t>(parsed)};
        if (CanTypeFitValue<std::int8_t>(number))
        {
            return Store(score::json::Number{static_cast<std::int8_t>(parsed)}).has_value();
        }

        if (CanTypeFitValue<std::int16_t>(number))
        {
            return Store(score::json::Number{static_cast<std::int16_t>(parsed)}).has_value();
        }

        if (CanTypeFitValue<std::int32_t>(number))
        {
            return Store(score::json::Number{static_cast<std::int32_t>(parsed)}).has_value();
        }

        if (CanTypeFitValue<std::int64_t>(number))
        {
            return Store(score::json::Number{static_cast<std::int64_t>(parsed)}).has_value();
        }
    }

    else if (parsed.is_number_float())
    {
        score::json::Number number_double{static_cast<double>(parsed)};
        if (number_double.As<double>().has_value())
        {
            return Store(number_double).has_value();
        }
    }

    return false;
}

auto score::json::JsonBuilder::GetData() -> score::Result<score::json::Any>
{
    score::Result<score::json::Any> tmp{};
    std::swap(tmp.value(), root_);
    return tmp;
}
