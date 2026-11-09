#include "sdv-hack-demo/vehicle_app/signal_test_values.h"

#include <fstream>
#include <iostream>

int main() {
    using namespace body_signal_test;
    try {
        std::ifstream input{"sdv-hack-demo/signal_routes.json"};
        const auto signals = LoadSignals(nlohmann::json::parse(input));
        if (signals.size() != 44U) {
            return 1;
        }
        for (const auto& signal : signals) {
            const auto values = TestValues(signal);
            if (values.empty() || values.front() != values.back()) {
                return 1;
            }
            for (const auto& value : values) {
                if (value.size() > 32U || DisplayValue(signal, value) == "<invalid payload>") {
                    return 1;
                }
            }
        }
        const Signal boolean{"boolean", "boolean", "boolean"};
        const Signal percentage{"percentage", "percentage", "uint8"};
        const Signal text{"text", "text", "string"};
        if (TestValues(boolean) != std::vector<Payload>{{0}, {1}, {0}} ||
            DisplayValue(boolean, {1}) != "true" || DisplayValue(boolean, {2}) != "<invalid payload>" ||
            TestValues(percentage) != std::vector<Payload>{{0}, {50}, {100}, {0}} ||
            DisplayValue(percentage, {100}) != "100" ||
            DisplayValue(text, TestValues(text)[1]) != "FAST" ||
            DisplayValue(text, Payload(33, 'A')) != "<invalid payload>") {
            return 1;
        }
        const nlohmann::json route{{"service", 17152}, {"apiName", "Vehicle.Test"},
                                   {"eventName", "test"}, {"datatype", "boolean"},
                                   {"maxPayloadSize", 1}};
        auto invalid = route;
        invalid["datatype"] = "unsupported";
        for (const auto& routes : {nlohmann::json::array(), nlohmann::json{route, route},
                                   nlohmann::json{invalid}}) {
            bool rejected = false;
            try {
                (void)LoadSignals(nlohmann::json{{"routes", routes}});
            } catch (const std::runtime_error&) {
                rejected = true;
            }
            if (!rejected) {
                return 1;
            }
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}