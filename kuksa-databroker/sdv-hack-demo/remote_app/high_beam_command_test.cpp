#include "high_beam_udp_protocol.h"

int main() {
    using high_beam_udp::CommandResult;
    high_beam_udp::Frame frame{17152, 4096, 33840, 1, {1}};
    unsigned int writes = 0;
    bool output = false;
    const auto write = [&](bool value) { ++writes; output = value; return true; };
    if (high_beam_udp::ApplyCommand(frame, true, write) != CommandResult::kApplied || !output || writes != 1) {
        return 1;
    }
    frame.payload[0] = 0;
    if (high_beam_udp::ApplyCommand(frame, true, write) != CommandResult::kApplied || output) {
        return 2;
    }
    const auto previous_writes = writes;
    frame.payload[0] = 1;
    if (high_beam_udp::ApplyCommand(frame, false, write) != CommandResult::kDisplayed || writes != previous_writes) {
        return 3;
    }
    if (high_beam_udp::ApplyCommand(frame, true, [](bool) { return false; }) != CommandResult::kOutputFailed) {
        return 4;
    }
    for (const unsigned char size : {0, 2, 32}) {
        frame.payload_size = size;
        if (high_beam_udp::ApplyCommand(frame, true, write) != CommandResult::kMalformed) { return 5; }
    }
    frame.payload_size = 1;
    frame.payload[0] = 2;
    if (high_beam_udp::ApplyCommand(frame, true, write) != CommandResult::kMalformed || writes != previous_writes) {
        return 6;
    }
    for (const bool value : {false, true}) {
        const auto update = high_beam_udp::BooleanUpdate(17152, 4097, 33849, value);
        const auto bytes = high_beam_udp::Encode(update);
        const auto decoded = high_beam_udp::Decode(bytes.data(), bytes.size());
        if (!decoded.has_value() || decoded->service != 17152 || decoded->instance != 4097 ||
            decoded->event != 33849 || decoded->payload_size != 1 ||
            decoded->payload[0] != static_cast<std::uint8_t>(value)) {
            return 7;
        }
    }
    if (high_beam_udp::TestPayloads("boolean") != std::vector<std::vector<std::uint8_t>>{{0}, {1}, {0}} ||
        high_beam_udp::TestPayloads("uint8") != std::vector<std::vector<std::uint8_t>>{{0}, {50}, {100}, {0}} ||
        high_beam_udp::TestPayloads("string") !=
            std::vector<std::vector<std::uint8_t>>{{'O', 'F', 'F'}, {'F', 'A', 'S', 'T'}, {'O', 'F', 'F'}} ||
        high_beam_udp::TestPayloads("float").size() != 4U ||
        high_beam_udp::TestPayloads("float")[1] !=
            std::vector<std::uint8_t>{0xC1U, 0x1CU, 0xF5U, 0xC3U} ||
        high_beam_udp::TestPayloads("int16").front() != std::vector<std::uint8_t>{0U, 20U} ||
        high_beam_udp::TestPayloads("float_array")[1].size() != 32U ||
        high_beam_udp::TestPayloads(
            "string", "Vehicle.Powertrain.Transmission.PerformanceMode") !=
            std::vector<std::vector<std::uint8_t>>{
                {'N', 'O', 'R', 'M', 'A', 'L'}, {'S', 'P', 'O', 'R', 'T'}, {'N', 'O', 'R', 'M', 'A', 'L'}} ||
        !high_beam_udp::TestPayloads("unknown").empty()) {
        return 8;
    }
    return 0;
}