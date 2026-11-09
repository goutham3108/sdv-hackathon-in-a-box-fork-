#include "sdv-hack-demo/include/speed_payload.h"

#include <array>

int main() {
    std::array<std::uint8_t, 9> payload{};
    const std::array<std::uint8_t, 9> expected{0x40, 0x5f, 0x40, 0, 0, 0, 0, 0, 3};
    vehicle_dynamics_wire::EncodeSpeed(125.0, 3, payload.data());
    if (payload != expected || vehicle_dynamics_wire::DecodeSpeed(payload.data()) != 125.0) {
        return 1;
    }
    vehicle_dynamics_wire::EncodeSpeed(0.0, 3, payload.data());
    if (vehicle_dynamics_wire::DecodeSpeed(payload.data()) != 0.0) {
        return 1;
    }
    vehicle_dynamics_wire::EncodeSpeed(-12.5, 3, payload.data());
    return vehicle_dynamics_wire::DecodeSpeed(payload.data()) == -12.5 ? 0 : 1;
}