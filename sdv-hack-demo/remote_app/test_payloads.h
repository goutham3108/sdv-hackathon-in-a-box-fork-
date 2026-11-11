// Copyright (c) 2026 Contributors to the Eclipse Foundation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef SDV_REMOTE_TEST_PAYLOADS_H_
#define SDV_REMOTE_TEST_PAYLOADS_H_

#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <string_view>
#include <vector>

namespace high_beam_udp {

inline std::vector<std::uint8_t> Int16Payload(const std::int16_t value) {
    const auto bits = static_cast<std::uint16_t>(value);
    return {static_cast<std::uint8_t>(bits >> 8U), static_cast<std::uint8_t>(bits)};
}

inline std::vector<std::uint8_t> FloatPayload(const float value) {
    std::uint32_t bits{};
    std::memcpy(&bits, &value, sizeof(bits));
    return {static_cast<std::uint8_t>(bits >> 24U), static_cast<std::uint8_t>(bits >> 16U),
            static_cast<std::uint8_t>(bits >> 8U), static_cast<std::uint8_t>(bits)};
}

inline std::vector<std::uint8_t> FloatArrayPayload(const std::initializer_list<float> values) {
    std::vector<std::uint8_t> payload;
    for (const float value : values) {
        const auto encoded = FloatPayload(value);
        payload.insert(payload.end(), encoded.begin(), encoded.end());
    }
    return payload;
}

inline std::vector<std::vector<std::uint8_t>> TestPayloads(
    const std::string_view datatype, const std::string_view api_name = {}) {
    if (datatype == "boolean") {
        return {{0U}, {1U}, {0U}};
    }
    if (datatype == "uint8") {
        return {{0U}, {50U}, {100U}, {0U}};
    }
    if (datatype == "int16") {
        return {Int16Payload(20), Int16Payload(85), Int16Payload(110), Int16Payload(20)};
    }
    if (datatype == "float") {
        return {FloatPayload(0.0F), FloatPayload(-9.81F), FloatPayload(3.25F), FloatPayload(0.0F)};
    }
    if (datatype == "float_array") {
        return {FloatArrayPayload({3.61F, 3.62F, 3.63F, 3.64F}),
                FloatArrayPayload({3.65F, 3.66F, 3.67F, 3.68F, 3.69F, 3.70F, 3.71F, 3.72F}),
                FloatArrayPayload({3.61F, 3.62F, 3.63F, 3.64F})};
    }
    if (datatype == "string") {
        if (api_name == "Vehicle.Powertrain.Transmission.PerformanceMode") {
            return {{'N', 'O', 'R', 'M', 'A', 'L'},
                    {'S', 'P', 'O', 'R', 'T'},
                    {'N', 'O', 'R', 'M', 'A', 'L'}};
        }
        return {{'O', 'F', 'F'}, {'F', 'A', 'S', 'T'}, {'O', 'F', 'F'}};
    }
    return {};
}

}  // namespace high_beam_udp

#endif  // SDV_REMOTE_TEST_PAYLOADS_H_
