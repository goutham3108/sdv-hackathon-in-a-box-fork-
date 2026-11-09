#include "sdv-hack-demo/bridge/service_registration.h"

#include <algorithm>

namespace {

struct Application {
    std::size_t events = 0;
    std::size_t offers = 0;
    std::size_t requests = 0;
    bool valid = true;
    std::set<std::pair<std::uint16_t, std::uint16_t>> offered;
    std::set<std::pair<std::uint16_t, std::uint16_t>> requested;

    void offer_event(std::uint16_t, std::uint16_t, std::uint16_t event,
                     const std::set<std::uint16_t>& groups, int,
                     std::chrono::milliseconds, bool, bool) {
        valid = valid && offers == 0U && requests == 0U && groups == std::set<std::uint16_t>{event};
        ++events;
    }

    void offer_service(std::uint16_t service, std::uint16_t instance,
                       std::uint8_t major, std::uint32_t minor) {
        valid = valid && events == 38U && requests == 0U && major == 1U && minor == 0U;
        valid = offered.emplace(service, instance).second && valid;
        ++offers;
    }

    void request_service(std::uint16_t service, std::uint16_t instance) {
        valid = valid && events == 38U && offers == 2U;
        valid = requested.emplace(service, instance).second && valid;
        ++requests;
    }
};

}

int main() {
    std::vector<signal_bridge::Route> routes{{"speed", 6433, 1, 2, 1, 1}};
    for (std::uint16_t index = 0; index < 38U; ++index) {
        if (index == 1U) {
            continue;
        }
        routes.push_back({"body", 17152, 4096, 4097,
                          static_cast<std::uint16_t>(33840U + index * 2U),
                          static_cast<std::uint16_t>(33841U + index * 2U)});
    }
    for (int order = 0; order < 2; ++order) {
        Application application;
        signal_bridge::RegisterServices(application, routes, 0, 1U, 0U);
        if (!application.valid || application.events != 38U || application.offers != 2U ||
            application.requests != 2U) {
            return 1;
        }
        std::reverse(routes.begin(), routes.end());
    }
    return 0;
}