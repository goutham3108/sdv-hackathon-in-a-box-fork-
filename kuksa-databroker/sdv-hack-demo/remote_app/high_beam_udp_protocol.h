// Local copy for remote app package

#ifndef D01C7798_D828_4851_832E_6E8136237E59
#define D01C7798_D828_4851_832E_6E8136237E59

#ifndef HIGH_BEAM_UDP_PROTOCOL_H_
#define HIGH_BEAM_UDP_PROTOCOL_H_

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <optional>
#include <string_view>
#include <vector>

namespace high_beam_udp {

constexpr std::uint32_t kMagic = 0x48424731U;
constexpr std::uint8_t kVersion = 1U;
constexpr std::size_t kHeaderSize = 14U;
constexpr std::size_t kMaxPayloadSize = 32U;
constexpr std::size_t kFrameSize = kHeaderSize + kMaxPayloadSize;

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

struct Frame {
    std::uint16_t service;
    std::uint16_t instance;
    std::uint16_t event;
    std::uint8_t payload_size;
    std::array<std::uint8_t, kMaxPayloadSize> payload{};
};

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

inline Frame BooleanUpdate(std::uint16_t service, std::uint16_t instance,
                           std::uint16_t event, bool value) {
    return Frame{service, instance, event, 1U, {static_cast<std::uint8_t>(value)}};
}

inline std::array<std::uint8_t, kFrameSize> Encode(const Frame& frame) {
    std::array<std::uint8_t, kFrameSize> bytes{};
    bytes[0] = static_cast<std::uint8_t>(kMagic >> 24U);
    bytes[1] = static_cast<std::uint8_t>(kMagic >> 16U);
    bytes[2] = static_cast<std::uint8_t>(kMagic >> 8U);
    bytes[3] = static_cast<std::uint8_t>(kMagic);
    bytes[4] = kVersion;
    bytes[6] = static_cast<std::uint8_t>(frame.service >> 8U);
    bytes[7] = static_cast<std::uint8_t>(frame.service);
    bytes[8] = static_cast<std::uint8_t>(frame.instance >> 8U);
    bytes[9] = static_cast<std::uint8_t>(frame.instance);
    bytes[10] = static_cast<std::uint8_t>(frame.event >> 8U);
    bytes[11] = static_cast<std::uint8_t>(frame.event);
    bytes[12] = frame.payload_size;
    std::copy(frame.payload.begin(), frame.payload.begin() + frame.payload_size, bytes.begin() + kHeaderSize);
    return bytes;
}

inline std::optional<Frame> Decode(const std::uint8_t* bytes, const std::size_t size) {
    if (size != kFrameSize || bytes[0] != static_cast<std::uint8_t>(kMagic >> 24U) ||
        bytes[1] != static_cast<std::uint8_t>(kMagic >> 16U) ||
        bytes[2] != static_cast<std::uint8_t>(kMagic >> 8U) || bytes[3] != static_cast<std::uint8_t>(kMagic) ||
        bytes[4] != kVersion || bytes[12] > kMaxPayloadSize) {
        return std::nullopt;
    }
    Frame frame{static_cast<std::uint16_t>((bytes[6] << 8U) | bytes[7]),
                static_cast<std::uint16_t>((bytes[8] << 8U) | bytes[9]),
                static_cast<std::uint16_t>((bytes[10] << 8U) | bytes[11]), bytes[12]};
    std::copy(bytes + kHeaderSize, bytes + kFrameSize, frame.payload.begin());
    return frame;
}

enum class CommandResult { kMalformed, kDisplayed, kApplied, kOutputFailed };

template <typename WriteOutput>
CommandResult ApplyCommand(const Frame& frame, bool output_available, WriteOutput write_output) {
    if (frame.payload_size != 1U || frame.payload[0] > 1U) {
        return CommandResult::kMalformed;
    }
    if (!output_available) {
        return CommandResult::kDisplayed;
    }
    return write_output(frame.payload[0] == 1U) ? CommandResult::kApplied : CommandResult::kOutputFailed;
}

}  // namespace high_beam_udp

#endif  // HIGH_BEAM_UDP_PROTOCOL_H_


#endif /* D01C7798_D828_4851_832E_6E8136237E59 */
