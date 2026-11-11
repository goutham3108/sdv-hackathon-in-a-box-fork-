#ifndef B0724761_709C_465F_AFC7_8603DC93DFAC
#define B0724761_709C_465F_AFC7_8603DC93DFAC
#ifndef HIGH_BEAM_UDP_PROTOCOL_H_
#define HIGH_BEAM_UDP_PROTOCOL_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace high_beam_udp {

constexpr std::uint32_t kMagic = 0x48424731U;
constexpr std::uint8_t kVersion = 1U;
constexpr std::size_t kFrameSize = 16U;

struct Frame {
    std::uint16_t service;
    std::uint16_t instance;
    std::uint16_t event;
    std::uint8_t value;
};

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
    bytes[12] = 1U;
    bytes[15] = frame.value;
    return bytes;
}

inline std::optional<Frame> Decode(const std::uint8_t* bytes, const std::size_t size) {
    if (size != kFrameSize || bytes[0] != static_cast<std::uint8_t>(kMagic >> 24U) ||
        bytes[1] != static_cast<std::uint8_t>(kMagic >> 16U) ||
        bytes[2] != static_cast<std::uint8_t>(kMagic >> 8U) || bytes[3] != static_cast<std::uint8_t>(kMagic) ||
        bytes[4] != kVersion || bytes[12] != 1U || bytes[15] > 1U) {
        return std::nullopt;
    }
    return Frame{static_cast<std::uint16_t>((bytes[6] << 8U) | bytes[7]),
                 static_cast<std::uint16_t>((bytes[8] << 8U) | bytes[9]),
                 static_cast<std::uint16_t>((bytes[10] << 8U) | bytes[11]), bytes[15]};
}

}  // namespace high_beam_udp

#endif  // HIGH_BEAM_UDP_PROTOCOL_H_


#endif /* B0724761_709C_465F_AFC7_8603DC93DFAC */
