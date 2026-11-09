/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

#include <atomic>
#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <vsomeip/vsomeip.hpp>

#include <kvsbuilder.hpp>
#include "../high_beam_udp_protocol.h"
#include "score/mw/lifecycle/report_running.h"
#include "score/mw/log/logging.h"

namespace {

constexpr vsomeip::service_t kService = 0x4300;
constexpr vsomeip::instance_t kVehicleInstance = 0x1000;
constexpr vsomeip::instance_t kRemoteInstance = 0x1001;
constexpr vsomeip::event_t kVehicleEvent = 0x8430;
constexpr vsomeip::event_t kRemoteEvent = 0x8431;
constexpr vsomeip::eventgroup_t kVehicleEventgroup = 0x8430;
constexpr vsomeip::eventgroup_t kRemoteEventgroup = 0x8431;
constexpr auto kPublishPeriod = std::chrono::seconds{2};
constexpr char kSensorStateKey[] = "high_beam_state";
constexpr std::uint16_t kDefaultBridgePort = 35000U;
constexpr std::uint16_t kDefaultRemotePort = 35001U;

std::optional<bool> ParseBooleanInput(const std::string& input) {
    if (input == "true") {
        return true;
    }
    if (input == "false") {
        return false;
    }
    return std::nullopt;
}

std::uint16_t ResolvePort(const char* name, const std::uint16_t fallback) {
    const char* value = std::getenv(name);
    return value == nullptr ? fallback : static_cast<std::uint16_t>(std::stoi(value));
}

std::string ResolveAddress(const char* name, const char* fallback) {
    const char* value = std::getenv(name);
    return value == nullptr || value[0] == '\0' ? fallback : value;
}

bool SetAddress(sockaddr_in& address, const std::string& value) {
    return inet_pton(AF_INET, value.c_str(), &address.sin_addr) == 1;
}

int CreateUdpSocket(const std::uint16_t port, const std::string& bind_ip) {
    const int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        return -1;
    }
    sockaddr_in address{};
    address.sin_family = AF_INET;
    if (!SetAddress(address, bind_ip)) {
        close(socket_fd);
        return -1;
    }
    address.sin_port = htons(port);
    if (bind(socket_fd, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) < 0) {
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}

}  // namespace

int main() {
    const char* const configured_kvs_dir = std::getenv("HIGH_BEAM_KVS_DIR");
    const std::string kvs_dir = configured_kvs_dir != nullptr && configured_kvs_dir[0] != '\0'
                                    ? configured_kvs_dir
                                    : "/tmp/score_high_beam_sensor";
    std::error_code filesystem_error;
    std::filesystem::create_directories(kvs_dir, filesystem_error);
    if (filesystem_error) {
        score::mw::log::LogError() << "Cannot create sensor KVS directory: " << kvs_dir;
        return 1;
    }
    score::mw::per::kvs::KvsBuilder kvs_builder{score::mw::per::kvs::InstanceId{1U}};
    kvs_builder.dir(std::string{kvs_dir});
    auto kvs_result = kvs_builder.build();
    if (!kvs_result.has_value()) {
        score::mw::log::LogError() << "Cannot open sensor KVS in " << kvs_dir;
        return 1;
    }
    auto sensor_store = std::make_shared<score::mw::per::kvs::Kvs>(std::move(kvs_result).value());
    std::atomic<bool> sensor_state{false};

    const std::uint16_t bridge_port = ResolvePort("HIGH_BEAM_BRIDGE_UDP_PORT", kDefaultBridgePort);
    const std::uint16_t remote_port = ResolvePort("HIGH_BEAM_REMOTE_UDP_PORT", kDefaultRemotePort);
    const std::string bind_ip = ResolveAddress("HIGH_BEAM_BIND_IP", "0.0.0.0");
    const std::string bridge_ip = ResolveAddress("HIGH_BEAM_BRIDGE_IP", "127.0.0.1");
    const int udp_socket = CreateUdpSocket(remote_port, bind_ip);
    if (udp_socket < 0) {
        score::mw::log::LogError() << "Failed to bind remote UDP port " << remote_port;
        return 1;
    }
    sockaddr_in bridge_address{};
    bridge_address.sin_family = AF_INET;
    if (!SetAddress(bridge_address, bridge_ip)) {
        score::mw::log::LogError() << "Invalid HIGH_BEAM_BRIDGE_IP: " << bridge_ip;
        close(udp_socket);
        return 1;
    }
    bridge_address.sin_port = htons(bridge_port);
    if (std::getenv("PROCESSIDENTIFIER") != nullptr) {
        score::mw::lifecycle::report_running();
    }

    std::thread input_thread([&sensor_state, &sensor_store]() {
        std::ifstream terminal_input{"/dev/tty"};
        std::istream& input_stream = terminal_input.is_open() ? static_cast<std::istream&>(terminal_input)
                                                               : std::cin;
        score::mw::log::LogWarn() << "Remote sensor input ready. Enter true or false to change its state.";
        std::string input;
        while (std::getline(input_stream, input)) {
            const auto value = ParseBooleanInput(input);
            if (!value.has_value()) {
                score::mw::log::LogWarn() << "Invalid remote sensor input. Enter true or false.";
                continue;
            }
            sensor_state.store(value.value());
            (void)sensor_store->set_value(kSensorStateKey, score::mw::per::kvs::KvsValue{value.value()});
            (void)sensor_store->flush();
            score::mw::log::LogWarn() << "Remote sensor input set High.IsOn="
                                      << (value.value() ? "true" : "false");
        }
    });
    input_thread.detach();

    std::thread receive_thread([udp_socket, &sensor_state, &sensor_store]() {
        std::array<std::uint8_t, high_beam_udp::kFrameSize> bytes{};
        while (true) {
            const auto received = recvfrom(udp_socket, bytes.data(), bytes.size(), 0, nullptr, nullptr);
            if (received <= 0) {
                continue;
            }
            const auto frame = high_beam_udp::Decode(bytes.data(), static_cast<std::size_t>(received));
            if (!frame.has_value() || frame->service != kService || frame->instance != kVehicleInstance ||
                frame->event != kVehicleEvent) {
                score::mw::log::LogError() << "Remote app received invalid UDP SOME/IP frame";
                continue;
            }
            auto payload = vsomeip::runtime::get()->create_payload();
            payload->set_data(std::vector<vsomeip::byte_t>{frame->value});
            const bool value = payload->get_data()[0] == 1U;
            sensor_state.store(value);
            (void)sensor_store->set_value(kSensorStateKey, score::mw::per::kvs::KvsValue{value});
            (void)sensor_store->flush();
            score::mw::log::LogWarn() << "Remote app converted UDP to SOME/IP High.IsOn="
                                      << (value ? "true" : "false");
        }
    });

    while (true) {
        const bool value = sensor_state.load();
        const auto frame = high_beam_udp::Encode({kService, kRemoteInstance, kRemoteEvent,
                                                  static_cast<std::uint8_t>(value)});
        (void)sendto(udp_socket, frame.data(), frame.size(), 0,
                     reinterpret_cast<const sockaddr*>(&bridge_address), sizeof(bridge_address));
        score::mw::log::LogWarn() << "Remote sensor converted SOME/IP to UDP High.IsOn="
                                  << (value ? "true" : "false");
        std::this_thread::sleep_for(kPublishPeriod);
    }
    receive_thread.join();
    close(udp_socket);
    return 0;
}
