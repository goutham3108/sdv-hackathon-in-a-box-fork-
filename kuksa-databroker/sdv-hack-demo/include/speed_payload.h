#ifndef SDV_HACK_DEMO_SPEED_PAYLOAD_H_
#define SDV_HACK_DEMO_SPEED_PAYLOAD_H_

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

namespace vehicle_dynamics_wire {

static_assert(sizeof(double) == 8 && std::numeric_limits<double>::is_iec559);

inline void EncodeSpeed(double value, std::uint8_t quality, std::uint8_t* payload) {
    std::uint64_t bits{};
    std::memcpy(&bits, &value, sizeof(bits));
    for (std::size_t index = 0; index < sizeof(bits); ++index) {
        payload[index] = static_cast<std::uint8_t>(bits >> ((7U - index) * 8U));
    }
    payload[8] = quality;
}

inline double DecodeSpeed(const std::uint8_t* payload) {
    std::uint64_t bits{};
    for (std::size_t index = 0; index < sizeof(bits); ++index) {
        bits = (bits << 8U) | payload[index];
    }
    double value{};
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

}

#endif