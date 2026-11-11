// Copyright (c) 2026 Contributors to the Eclipse Foundation
//
// SPDX-License-Identifier: Apache-2.0

#include "sdv-hack-demo/include/high_beam_udp_protocol.h"

int main() {
    using namespace high_beam_udp;
    static_assert(kFrameSize == 46U);
    const Frame frame{0x4300U, 0x1001U, 0x8431U, 1U, {1U}};
    std::array<std::uint8_t, 46U> expected{
        0x48U, 0x42U, 0x47U, 0x31U, 1U, 0U,
        0x43U, 0U, 0x10U, 1U, 0x84U, 0x31U, 1U, 0U, 1U};
    if (Encode(frame) != expected) {
        return 1;
    }
    for (std::size_t length = 0; length <= kMaxPayloadSize; ++length) {
        Frame sample{0x4300U, 0x1001U, 0x8431U, static_cast<std::uint8_t>(length), {}};
        for (std::size_t index = 0; index < length; ++index) {
            sample.payload[index] = static_cast<std::uint8_t>(index + 1U);
        }
        const auto bytes = Encode(sample);
        const auto decoded = Decode(bytes.data(), bytes.size());
        if (!decoded || decoded->service != sample.service || decoded->instance != sample.instance ||
            decoded->event != sample.event || decoded->payload_size != sample.payload_size ||
            decoded->payload != sample.payload) {
            return 2;
        }
    }
    for (std::size_t size = 0; size < kFrameSize; ++size) {
        if (Decode(expected.data(), size)) {
            return 3;
        }
    }
    if (Decode(expected.data(), kFrameSize + 1U)) {
        return 4;
    }
    for (const std::size_t index : {0U, 1U, 2U, 3U, 4U}) {
        auto invalid = expected;
        invalid[index] ^= 0xFFU;
        if (Decode(invalid.data(), invalid.size())) {
            return 5;
        }
    }
    expected[12] = kMaxPayloadSize + 1U;
    if (Decode(expected.data(), expected.size())) {
        return 6;
    }
    return 0;
}
