// Copy of the upstream SOME/IP bridge integration example.
// Packaged here so the demo can be built and packaged entirely from sdv-hack-demo.

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <array>
#include <arpa/inet.h>
#include <cstring>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>
#include <vsomeip/vsomeip.hpp>

#include "high_beam_udp_protocol.h"
#include "sdv-hack-demo/bridge/service_registration.h"
#include "sdv-hack-demo/include/speed_payload.h"
#include "score/mw/lifecycle/report_running.h"
#include "score/mw/log/logging.h"

namespace {

constexpr std::uint16_t kDefaultBridgePort = 35000U;
constexpr std::uint16_t kDefaultRemotePort = 35001U;
// The gateway subscribes with the service_version_major of its config (1); vsomeip only
// delivers events if the offered major version matches.
constexpr vsomeip::major_version_t kGatewayServiceMajorVersion = 1U;
constexpr vsomeip::minor_version_t kGatewayServiceMinorVersion = 0U;

using signal_bridge::Route;

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

std::string ResolveRouteConfigPath() {
    const char* const configured = std::getenv("SIGNAL_ROUTE_CONFIG");
    if (configured != nullptr && configured[0] != '\0') {
        return configured;
    }
    return "signal_routes.json";
}

std::optional<std::vector<Route>> LoadRoutes() {
    std::ifstream input{ResolveRouteConfigPath()};
    if (!input.is_open()) {
        score::mw::log::LogError() << "Cannot open signal route configuration";
        return std::nullopt;
    }
    try {
        const auto document = nlohmann::json::parse(input);
        std::vector<Route> routes;
        for (const auto& item : document.at("routes")) {
            routes.push_back(Route{item.at("name").get<std::string>(),
                                   item.at("service").get<vsomeip::service_t>(),
                                   item.at("vehicleInstance").get<vsomeip::instance_t>(),
                                   item.at("remoteInstance").get<vsomeip::instance_t>(),
                                   item.at("vehicleEvent").get<vsomeip::event_t>(),
                                   item.at("remoteEvent").get<vsomeip::event_t>()});
        }
        return routes;
    } catch (const std::exception& error) {
        score::mw::log::LogError() << "Invalid signal route configuration: " << error.what();
        return std::nullopt;
    }
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
    const auto routes = LoadRoutes();
    if (!routes.has_value() || routes->empty()) {
        return 1;
    }
    const std::string vehicle_domain_config =
        ResolveConfigPath("VEHICLE_DOMAIN_CONFIG", "tests/integration/vsomeip-gateway-services.json");
    if (!std::filesystem::exists(vehicle_domain_config)) {
        score::mw::log::LogError() << "Bridge cannot locate domain configuration files";
        return 1;
    }

    auto vehicle_side = vsomeip::runtime::get()->create_application(
        "udp_bridge_vehicle_side", vehicle_domain_config);
    if (!vehicle_side || !vehicle_side->init()) {
        score::mw::log::LogError() << "Failed to initialize bridge SOME/IP applications";
        return 1;
    }
    const std::uint16_t bridge_port = ResolvePort("SDV_BRIDGE_UDP_PORT", kDefaultBridgePort);
    const std::uint16_t remote_port = ResolvePort("SDV_REMOTE_UDP_PORT", kDefaultRemotePort);
    const std::string bind_ip = ResolveAddress("SDV_BIND_IP", "0.0.0.0");
    const std::string remote_ip = ResolveAddress("SDV_REMOTE_IP", "127.0.0.1");
    const int udp_socket = CreateUdpSocket(bridge_port, bind_ip);
    if (udp_socket < 0) {
        score::mw::log::LogError() << "Failed to bind bridge UDP port " << bridge_port;
        return 1;
    }
    sockaddr_in remote_address{};
    remote_address.sin_family = AF_INET;
    if (!SetAddress(remote_address, remote_ip)) {
        score::mw::log::LogError() << "Invalid SDV_REMOTE_IP: " << remote_ip;
        close(udp_socket);
        return 1;
    }
    remote_address.sin_port = htons(remote_port);

    vehicle_side->register_state_handler([&vehicle_side, &routes](const vsomeip::state_type_e state) {
        if (state != vsomeip::state_type_e::ST_REGISTERED) {
            return;
        }
        signal_bridge::RegisterServices(*vehicle_side, *routes, vsomeip::event_type_e::ET_EVENT,
                                       kGatewayServiceMajorVersion, kGatewayServiceMinorVersion);
        for (const auto& route : *routes) {
            score::mw::log::LogWarn() << "Bridge route ready: " << route.name;
        }
    });

    std::set<std::pair<vsomeip::service_t, vsomeip::instance_t>> registered_services;
    for (const auto& route : *routes) {
        vehicle_side->register_subscription_status_handler(
            route.service, route.vehicle_instance, route.vehicle_event, route.vehicle_event,
            [route](vsomeip::service_t, vsomeip::instance_t, vsomeip::eventgroup_t,
                    vsomeip::event_t, const std::uint16_t error) {
                if (error == 0U) {
                    score::mw::log::LogWarn() << "Bridge subscription ACK: " << route.name;
                } else {
                    score::mw::log::LogError() << "Bridge subscription NACK: " << route.name
                                              << " error=" << error;
                }
            });
        if (registered_services.emplace(route.service, route.vehicle_instance).second) {
            vehicle_side->register_availability_handler(
            route.service, route.vehicle_instance,
            [&vehicle_side, &routes](const vsomeip::service_t service, const vsomeip::instance_t instance,
                                    const bool available) {
            if (!available) {
                return;
            }
            for (const auto& candidate : *routes) {
                if (candidate.service != service || candidate.vehicle_instance != instance) {
                    continue;
                }
                const std::set<vsomeip::eventgroup_t> vehicle_groups{candidate.vehicle_event};
                vehicle_side->request_event(service, instance, candidate.vehicle_event, vehicle_groups,
                                            vsomeip::event_type_e::ET_EVENT);
                vehicle_side->subscribe(service, instance, candidate.vehicle_event);
                score::mw::log::LogWarn() << "Bridge subscribed to " << candidate.name;
            }
        });
        }

        vehicle_side->register_message_handler(
        route.service, route.vehicle_instance, route.vehicle_event,
        [&remote_address, udp_socket, route](const std::shared_ptr<vsomeip::message>& message) {
            if (!IsBooleanPayload(message)) {
                const auto payload = message->get_payload();
                if (payload->get_length() > high_beam_udp::kMaxPayloadSize) {
                    score::mw::log::LogError() << "Bridge received oversized payload for " << route.name;
                    return;
                }
            }
            const auto payload = message->get_payload();
            high_beam_udp::Frame outbound{route.service, route.vehicle_instance, route.vehicle_event,
                                          static_cast<std::uint8_t>(payload->get_length()), {}};
            std::copy(payload->get_data(), payload->get_data() + payload->get_length(), outbound.payload.begin());
            const auto frame = high_beam_udp::Encode(outbound);
            (void)sendto(udp_socket, frame.data(), frame.size(), 0,
                         reinterpret_cast<const sockaddr*>(&remote_address), sizeof(remote_address));
            auto&& trace = score::mw::log::LogWarn();
            trace << "Bridge forwarded " << route.name
                  << " vehicle-to-remote payload_size=" << payload->get_length();
            if (payload->get_length() == sizeof(double) + 1U) {
                const double value = vehicle_dynamics_wire::DecodeSpeed(payload->get_data());
                trace << " value=" << value;
            }
        });
            }

    std::thread udp_thread([&vehicle_side, udp_socket, &routes]() {
        std::array<std::uint8_t, high_beam_udp::kFrameSize> bytes{};
        while (true) {
            sockaddr_in sender{};
            socklen_t sender_size = sizeof(sender);
            const auto received = recvfrom(udp_socket, bytes.data(), bytes.size(), 0,
                                           reinterpret_cast<sockaddr*>(&sender), &sender_size);
            if (received <= 0) {
                continue;
            }
            char sender_ip[INET_ADDRSTRLEN]{};
            const char* const sender_address =
                inet_ntop(AF_INET, &sender.sin_addr, sender_ip, sizeof(sender_ip));
            score::mw::log::LogWarn() << "Bridge received UDP frame from "
                                      << (sender_address == nullptr ? "<unknown>" : sender_address)
                                      << ":" << ntohs(sender.sin_port) << ", size=" << received;
            const auto frame = high_beam_udp::Decode(bytes.data(), static_cast<std::size_t>(received));
            if (!frame.has_value()) {
                score::mw::log::LogError() << "Bridge received invalid UDP SOME/IP frame";
                continue;
            }
            const auto route = std::find_if(routes->begin(), routes->end(), [&frame](const Route& candidate) {
                return frame->service == candidate.service && frame->instance == candidate.remote_instance &&
                       frame->event == candidate.remote_event;
            });
            if (!frame.has_value() || route == routes->end()) {
                score::mw::log::LogError() << "Bridge received invalid UDP SOME/IP frame";
                continue;
            }
            auto payload = vsomeip::runtime::get()->create_payload();
            payload->set_data(std::vector<vsomeip::byte_t>(frame->payload.begin(),
                                                           frame->payload.begin() + frame->payload_size));
            vehicle_side->notify(route->service, route->remote_instance, route->remote_event, payload);
            if (frame->payload_size == sizeof(double) + 1U) {
                const double value = vehicle_dynamics_wire::DecodeSpeed(frame->payload.data());
                score::mw::log::LogWarn() << "Bridge converted UDP to SOME/IP " << route->name
                                          << " value=" << value;
            } else {
                score::mw::log::LogWarn() << "Bridge converted UDP to SOME/IP " << route->name
                                          << " payload_size=" << frame->payload_size;
            }
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
