#ifndef SDV_HACK_DEMO_BRIDGE_SERVICE_REGISTRATION_H
#define SDV_HACK_DEMO_BRIDGE_SERVICE_REGISTRATION_H

#include <chrono>
#include <cstdint>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace signal_bridge {

struct Route {
    std::string name;
    std::uint16_t service;
    std::uint16_t vehicle_instance;
    std::uint16_t remote_instance;
    std::uint16_t vehicle_event;
    std::uint16_t remote_event;
};

template <typename Application, typename EventType>
void RegisterServices(Application& application, const std::vector<Route>& routes,
                      EventType event_type, std::uint8_t major_version, std::uint32_t minor_version) {
    for (const auto& route : routes) {
        const std::set<std::uint16_t> groups{route.remote_event};
        application.offer_event(route.service, route.remote_instance, route.remote_event, groups,
                                event_type, std::chrono::milliseconds::zero(), false, true);
    }
    std::set<std::pair<std::uint16_t, std::uint16_t>> offered;
    for (const auto& route : routes) {
        if (offered.emplace(route.service, route.remote_instance).second) {
            application.offer_service(route.service, route.remote_instance, major_version, minor_version);
        }
    }
    std::set<std::pair<std::uint16_t, std::uint16_t>> requested;
    for (const auto& route : routes) {
        if (requested.emplace(route.service, route.vehicle_instance).second) {
            application.request_service(route.service, route.vehicle_instance);
        }
    }
}

}

#endif