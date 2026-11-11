#ifndef SDV_HACK_DEMO_SIGNAL_TEST_VALUES_H
#define SDV_HACK_DEMO_SIGNAL_TEST_VALUES_H

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

namespace body_signal_test {

struct Signal {
    std::string api_name;
    std::string event_name;
    std::string datatype;
    std::size_t max_payload_size{1U};
};

using Payload = std::vector<std::uint8_t>;

inline std::vector<Signal> LoadSignals(const nlohmann::json& document) {
    std::vector<Signal> signals;
    for (const auto& route : document.at("routes")) {
        if (route.at("service").get<std::uint16_t>() != 17152U) {
            continue;
        }
        Signal signal{route.at("apiName").get<std::string>(),
                      route.at("eventName").get<std::string>(),
                      route.at("datatype").get<std::string>(),
                      route.at("maxPayloadSize").get<std::size_t>()};
        if (signal.datatype != "boolean" && signal.datatype != "uint8" && signal.datatype != "int16" &&
            signal.datatype != "float" && signal.datatype != "float_array" && signal.datatype != "string") {
            throw std::runtime_error("Unsupported datatype for " + signal.api_name);
        }
        for (const auto& previous : signals) {
            if (previous.event_name == signal.event_name || previous.api_name == signal.api_name) {
                throw std::runtime_error("Duplicate body signal: " + signal.api_name);
            }
        }
        signals.push_back(std::move(signal));
    }
    if (signals.empty()) {
        throw std::runtime_error("No body signals in route configuration");
    }
    return signals;
}

inline std::vector<Payload> TestValues(const Signal& signal) {
    if (signal.api_name == "Vehicle.Powertrain.Transmission.PerformanceMode") {
        return {{'N', 'O', 'R', 'M', 'A', 'L'}, {'S', 'P', 'O', 'R', 'T'}, {'N', 'O', 'R', 'M', 'A', 'L'}};
    }
    if (signal.datatype == "boolean") {
        return {{0U}, {1U}, {0U}};
    }
    if (signal.datatype == "uint8") {
        return {{0U}, {50U}, {100U}, {0U}};
    }
    if (signal.datatype == "int16") {
        return {{0U, 20U}, {0U, 85U}, {0U, 110U}, {0U, 20U}};
    }
    if (signal.datatype == "float") {
        return {{0U, 0U, 0U, 0U}, {0xC1U, 0x1CU, 0xCCU, 0xCDU}, {0x40U, 0x50U, 0U, 0U}, {0U, 0U, 0U, 0U}};
    }
    if (signal.datatype == "float_array") {
        return {{0x40U, 0x66U, 0x66U, 0x66U, 0x40U, 0x67U, 0x0AU, 0x3DU},
                {0x40U, 0x69U, 0x99U, 0x9AU, 0x40U, 0x6AU, 0x3DU, 0x71U,
                 0x40U, 0x6AU, 0xE1U, 0x48U, 0x40U, 0x6BU, 0x85U, 0x1FU},
                {0x40U, 0x66U, 0x66U, 0x66U, 0x40U, 0x67U, 0x0AU, 0x3DU}};
    }
    if (signal.datatype == "string") {
        return {{'O', 'F', 'F'}, {'F', 'A', 'S', 'T'}, {'O', 'F', 'F'}};
    }
    throw std::runtime_error("Unsupported datatype for " + signal.api_name);
}

inline std::string DisplayValue(const Signal& signal, const Payload& payload) {
    if (signal.datatype == "boolean" && payload.size() == 1U && payload[0] <= 1U) {
        return payload[0] == 0U ? "false" : "true";
    }
    if (signal.datatype == "uint8" && payload.size() == 1U) {
        return std::to_string(payload[0]);
    }
    if (signal.datatype == "int16" && payload.size() == 2U) {
        const auto bits = static_cast<std::uint16_t>((payload[0] << 8U) | payload[1]);
        return std::to_string(static_cast<std::int16_t>(bits));
    }
    if (signal.datatype == "float" && payload.size() == 4U) {
        const std::uint32_t bits = (static_cast<std::uint32_t>(payload[0]) << 24U) |
                                   (static_cast<std::uint32_t>(payload[1]) << 16U) |
                                   (static_cast<std::uint32_t>(payload[2]) << 8U) | payload[3];
        float value{};
        std::memcpy(&value, &bits, sizeof(value));
        return std::to_string(value);
    }
    if (signal.datatype == "float_array" && payload.size() <= 32U && payload.size() % 4U == 0U) {
        std::ostringstream result;
        result << '[';
        for (std::size_t offset = 0; offset < payload.size(); offset += 4U) {
            if (offset != 0U) result << ", ";
            const std::uint32_t bits = (static_cast<std::uint32_t>(payload[offset]) << 24U) |
                                       (static_cast<std::uint32_t>(payload[offset + 1U]) << 16U) |
                                       (static_cast<std::uint32_t>(payload[offset + 2U]) << 8U) |
                                       payload[offset + 3U];
            float value{};
            std::memcpy(&value, &bits, sizeof(value));
            result << std::fixed << std::setprecision(2) << value;
        }
        result << ']';
        return result.str();
    }
    if (signal.datatype == "string" && payload.size() <= 32U) {
        return std::string{payload.begin(), payload.end()};
    }
    return "<invalid payload>";
}

}

#endif