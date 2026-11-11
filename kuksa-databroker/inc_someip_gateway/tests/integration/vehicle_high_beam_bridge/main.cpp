/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

// Runs only on the vehicle/RPi side. Bridges the vehicle-local SOME/IP domain
// (shared with someipd/gatewayd) and the isolated remote-device SOME/IP domain,
// preserving the same service ID (0x4300) on both domains. Vehicle-origin state
// uses instance 0x1000/event 0x8430; remote-origin state uses instance
// 0x1001/event 0x8431, so a relayed message is never re-forwarded to its origin.

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <array>
#include <arpa/inet.h>
#include <cstring>
#include <set>
#include <string>
#include <thread>

#include <vsomeip/vsomeip.hpp>

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
constexpr std::uint16_t kDefaultBridgePort = 35000U;
constexpr std::uint16_t kDefaultRemotePort = 35001U;

bool IsBooleanPayload(const std::shared_ptr<vsomeip::message>& message) {
    const auto payload = message->get_payload();
    return payload->get_length() == 1U &&
           (payload->get_data()[0] == static_cast<vsomeip::byte_t>(0x00) ||
            payload->get_data()[0] == static_cast<vsomeip::byte_t>(0x01));
}

std::string ResolveConfigPath(const char* const environment_variable, const std::string& default_path) {
    const char* const override_path = std::getenv(environment_variable);
    if (override_path != nullptr && override_path[0] != '\0') {
        return override_path;
    }
    const char* const standard_path = std::getenv("VSOMEIP_CONFIGURATION");
    if (standard_path != nullptr && standard_path[0] != '\0') {
        return standard_path;
    }
    return default_path;
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
    const std::string vehicle_domain_config =
        ResolveConfigPath("VEHICLE_DOMAIN_CONFIG", "tests/integration/vsomeip-gateway-services.json");
    if (!std::filesystem::exists(vehicle_domain_config)) {
        score::mw::log::LogError() << "Bridge cannot locate domain configuration files";
        return 1;
    }

    auto vehicle_side = vsomeip::runtime::get()->create_application(
        "vehicle_high_beam_bridge_vehicle_side", vehicle_domain_config);
    if (!vehicle_side || !vehicle_side->init()) {
        score::mw::log::LogError() << "Failed to initialize bridge SOME/IP applications";
        return 1;
    }
    const std::uint16_t bridge_port = ResolvePort("HIGH_BEAM_BRIDGE_UDP_PORT", kDefaultBridgePort);
    const std::uint16_t remote_port = ResolvePort("HIGH_BEAM_REMOTE_UDP_PORT", kDefaultRemotePort);
    const std::string bind_ip = ResolveAddress("HIGH_BEAM_BIND_IP", "0.0.0.0");
    const std::string remote_ip = ResolveAddress("HIGH_BEAM_REMOTE_IP", "127.0.0.1");
    const int udp_socket = CreateUdpSocket(bridge_port, bind_ip);
    if (udp_socket < 0) {
        score::mw::log::LogError() << "Failed to bind bridge UDP port " << bridge_port;
        return 1;
    }
    sockaddr_in remote_address{};
    remote_address.sin_family = AF_INET;
    if (!SetAddress(remote_address, remote_ip)) {
        score::mw::log::LogError() << "Invalid HIGH_BEAM_REMOTE_IP: " << remote_ip;
        close(udp_socket);
        return 1;
    }
    remote_address.sin_port = htons(remote_port);

    // Vehicle-side leg: subscribe to the Vehicle app's state and offer the
    // remote-origin event that gatewayd's Rx route expects on this domain.
    vehicle_side->register_state_handler([&vehicle_side](const vsomeip::state_type_e state) {
        if (state != vsomeip::state_type_e::ST_REGISTERED) {
            return;
        }
        vehicle_side->request_service(kService, kVehicleInstance);

        const std::set<vsomeip::eventgroup_t> remote_groups{kRemoteEventgroup};
        vehicle_side->offer_event(kService, kRemoteInstance, kRemoteEvent, remote_groups,
                                  vsomeip::event_type_e::ET_EVENT, std::chrono::milliseconds::zero(), false, true);
        vehicle_side->offer_service(kService, kRemoteInstance);
        score::mw::log::LogWarn() << "Bridge vehicle-side leg ready [4300.1000/1001]";
    });

    vehicle_side->register_availability_handler(
        kService, kVehicleInstance,
        [&vehicle_side](const vsomeip::service_t, const vsomeip::instance_t, const bool available) {
            if (!available) {
                return;
            }
            const std::set<vsomeip::eventgroup_t> vehicle_groups{kVehicleEventgroup};
            vehicle_side->request_event(kService, kVehicleInstance, kVehicleEvent, vehicle_groups,
                                        vsomeip::event_type_e::ET_EVENT);
            vehicle_side->subscribe(kService, kVehicleInstance, kVehicleEventgroup);
            score::mw::log::LogWarn() << "Bridge subscribed to vehicle high-beam updates";
        });

    vehicle_side->register_availability_handler(
        kService, kRemoteInstance,
        [](const vsomeip::service_t, const vsomeip::instance_t, const bool available) {
            score::mw::log::LogWarn() << "Bridge vehicle-side reverse service available="
                                      << (available ? "true" : "false");
        });

    vehicle_side->register_message_handler(
        kService, kVehicleInstance, kVehicleEvent,
        [&remote_address, udp_socket](const std::shared_ptr<vsomeip::message>& message) {
            if (!IsBooleanPayload(message)) {
                score::mw::log::LogError() << "Bridge received invalid vehicle-origin payload";
                return;
            }
            const bool value = message->get_payload()->get_data()[0] == 0x01U;
            const auto frame = high_beam_udp::Encode({kService, kVehicleInstance, kVehicleEvent,
                                                      static_cast<std::uint8_t>(value)});
            (void)sendto(udp_socket, frame.data(), frame.size(), 0,
                         reinterpret_cast<const sockaddr*>(&remote_address), sizeof(remote_address));
            score::mw::log::LogWarn() << "[bridge] Vehicle->Remote: event=0x8430 value="
                                      << (value ? "true" : "false");
            score::mw::log::LogWarn()
                << "Bridge forwarded vehicle-to-remote High.IsOn="
                << (message->get_payload()->get_data()[0] == 0x01U ? "true" : "false");
        });

    std::thread udp_thread([&vehicle_side, udp_socket]() {
        std::array<std::uint8_t, high_beam_udp::kFrameSize> bytes{};
        while (true) {
            const auto received = recvfrom(udp_socket, bytes.data(), bytes.size(), 0, nullptr, nullptr);
            if (received <= 0) {
                continue;
            }
            score::mw::log::LogWarn() << "Bridge received UDP frame, size=" << received;
            const auto frame = high_beam_udp::Decode(bytes.data(), static_cast<std::size_t>(received));
            if (!frame.has_value() || frame->service != kService || frame->instance != kRemoteInstance ||
                frame->event != kRemoteEvent) {
                score::mw::log::LogError() << "Bridge received invalid UDP SOME/IP frame";
                continue;
            }
            auto payload = vsomeip::runtime::get()->create_payload();
            payload->set_data(std::vector<vsomeip::byte_t>{frame->value});
            vehicle_side->notify(kService, kRemoteInstance, kRemoteEvent, payload);
            score::mw::log::LogWarn() << "Bridge converted UDP to SOME/IP High.IsOn="
                                      << (frame->value == 1U ? "true" : "false");
        }
    });
    if (std::getenv("PROCESSIDENTIFIER") != nullptr) {
        score::mw::lifecycle::report_running();
    }
    vehicle_side->start();
    udp_thread.join();
    close(udp_socket);
    return 0;
}
