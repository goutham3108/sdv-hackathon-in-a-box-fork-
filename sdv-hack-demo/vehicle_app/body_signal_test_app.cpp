// Copyright (c) 2026 Contributors to the Eclipse Foundation
//
// SPDX-License-Identifier: Apache-2.0

#include "sdv-hack-demo/vehicle_app/signal_test_values.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <string_view>
#include <thread>
#include <utility>

#include "score/mw/com/runtime.h"
#include "score/mw/com/types.h"
#include "score/serializer/pre_serialized_data.h"

namespace {

using body_signal_test::Payload;
using body_signal_test::Signal;
using RawSample = score::someip_gateway::serializer::PreSerializedData<0>;
using Clock = std::chrono::steady_clock;

struct Options {
    std::string configuration{"mw_com_config.json"};
    std::string routes{"signal_routes.json"};
    std::chrono::milliseconds timeout{3000};
    std::chrono::milliseconds startup_timeout{20000};
    bool list{false};
};

class SignalTestApplication {
  public:
    SignalTestApplication(Options options, std::vector<Signal> signals)
        : options_{std::move(options)}, signals_{std::move(signals)} {}

    ~SignalTestApplication() {
        for (auto* event : subscribed_events_) {
            event->Unsubscribe();
        }
        if (skeleton_.has_value()) {
            skeleton_->StopOfferService();
        }
    }

    int Run() {
        score::mw::com::runtime::InitializeRuntime(
            score::mw::com::runtime::RuntimeConfiguration{options_.configuration});
        if (!ConnectReceiver() || !OfferSender()) {
            return EXIT_FAILURE;
        }
        std::size_t passed = 0;
        for (const auto& signal : signals_) {
            bool success = true;
            auto tx_events = skeleton_->GetEvents();
            auto rx_events = proxy_->GetEvents();
            const auto tx = tx_events.find(signal.event_name);
            const auto rx = rx_events.find(signal.event_name);
            if (tx == tx_events.cend() || rx == rx_events.cend()) {
                std::cerr << "Missing event: " << signal.event_name << '\n';
                success = false;
            } else if (!rx->second.Subscribe(4U).has_value()) {
                std::cerr << "Cannot subscribe: " << signal.api_name << '\n';
                success = false;
            } else {
                subscribed_events_.push_back(&rx->second);
                const auto ready_deadline = Clock::now() + options_.startup_timeout;
                while (rx->second.GetSubscriptionState() != score::mw::com::SubscriptionState::kSubscribed &&
                       Clock::now() < ready_deadline) {
                    std::this_thread::sleep_for(std::chrono::milliseconds{50});
                }
                if (rx->second.GetSubscriptionState() != score::mw::com::SubscriptionState::kSubscribed) {
                    std::cerr << "Subscription timed out: " << signal.api_name << '\n';
                    success = false;
                } else {
                    for (const auto& value : body_signal_test::TestValues(signal)) {
                        const bool matched = TestValue(signal, value, tx->second, rx->second);
                        success = matched && success;
                    }
                }
            }
            std::cout << (success ? "PASS " : "FAIL ") << signal.api_name << std::endl;
            passed += success ? 1U : 0U;
        }
        std::cout << "Summary: " << passed << '/' << signals_.size() << " signals passed; "
                  << signals_.size() - passed << " failed.\n";
        return passed == signals_.size() ? EXIT_SUCCESS : EXIT_FAILURE;
    }

  private:
    bool ConnectReceiver() {
        const auto instance = score::mw::com::InstanceSpecifier::Create(
            std::string{"/body_control_module/network_rx"});
        if (!instance.has_value()) {
            return false;
        }
        const auto deadline = Clock::now() + options_.startup_timeout;
        while (Clock::now() < deadline) {
            const auto handles = score::mw::com::GenericProxy::FindService(instance.value());
            if (handles.has_value() && !handles.value().empty()) {
                auto result = score::mw::com::GenericProxy::Create(handles.value().front());
                if (result.has_value()) {
                    proxy_.emplace(std::move(result).value());
                    return true;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
        }
        std::cerr << "Cannot discover gateway Rx service; start gatewayd and someipd first.\n";
        return false;
    }

    bool OfferSender() {
        std::vector<score::mw::com::EventInfo> events;
        for (const auto& signal : signals_) {
            const std::size_t capacity = signal.max_payload_size;
            const score::mw::com::DataTypeMetaInfo metadata{
                score::someip_gateway::serializer::get_size_of_pre_serialized_data(capacity),
                alignof(RawSample)};
            events.push_back({signal.event_name, metadata});
        }
        const auto instance = score::mw::com::InstanceSpecifier::Create(
            std::string{"/body_control_module/local_tx"});
        if (!instance.has_value()) {
            return false;
        }
        score::mw::com::GenericSkeletonServiceElementInfo parameters;
        parameters.events = events;
        auto result = score::mw::com::GenericSkeleton::Create(instance.value(), parameters);
        if (!result.has_value()) {
            std::cerr << "Cannot create Tx service; stop KUKSA and the legacy vehicle publisher.\n";
            return false;
        }
        skeleton_.emplace(std::move(result).value());
        if (!skeleton_->OfferService().has_value()) {
            std::cerr << "Cannot offer body-signal Tx service.\n";
            return false;
        }
        return true;
    }

    bool TestValue(const Signal& signal, const Payload& expected,
                   score::mw::com::GenericSkeletonEvent& tx, score::mw::com::GenericProxyEvent& rx) {
        (void)rx.GetNewSamples([](score::mw::com::SamplePtr<void>) noexcept {}, 4U);
        bool matched = false;
        const auto deadline = Clock::now() + options_.timeout;
        auto next_send = Clock::now();
        std::cout << "TX " << signal.api_name << '=' << body_signal_test::DisplayValue(signal, expected)
                  << std::endl;
        while (Clock::now() < deadline) {
            if (Clock::now() >= next_send) {
                auto result = tx.Allocate();
                if (!result.has_value()) {
                    std::cerr << "Allocation failed: " << signal.api_name << '\n';
                    return false;
                }
                auto sample = std::move(result).value();
                auto* raw = static_cast<RawSample*>(sample.Get());
                raw->size = expected.size();
                std::memcpy(raw->data, expected.data(), expected.size());
                if (!tx.Send(std::move(sample)).has_value()) {
                    std::cerr << "Send failed: " << signal.api_name << '\n';
                    return false;
                }
                next_send = Clock::now() + std::chrono::milliseconds{500};
            }
            const auto received = rx.GetNewSamples(
                [&](score::mw::com::SamplePtr<void> sample) noexcept {
                    if (sample.Get() == nullptr) {
                        return;
                    }
                    Payload actual;
                    if (signal.datatype == "boolean") {
                        actual.push_back(*static_cast<const bool*>(sample.Get()) ? 1U : 0U);
                    } else {
                        const auto* raw = static_cast<const RawSample*>(sample.Get());
                        const std::size_t capacity = signal.max_payload_size;
                        if (raw->size > capacity) {
                            return;
                        }
                        const auto* bytes = reinterpret_cast<const std::uint8_t*>(raw->data);
                        actual.assign(bytes, bytes + raw->size);
                    }
                    if (actual == expected) {
                        matched = true;
                    }
                }, 4U);
            if (!received.has_value()) {
                std::cerr << "Receive failed: " << signal.api_name << '\n';
                return false;
            }
            if (matched) {
                std::cout << "RX " << signal.api_name << '=' << body_signal_test::DisplayValue(signal, expected)
                          << std::endl;
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds{20});
        }
        std::cerr << "TIMEOUT " << signal.api_name << '=' << body_signal_test::DisplayValue(signal, expected)
                  << '\n';
        return false;
    }

    Options options_;
    std::vector<Signal> signals_;
    std::optional<score::mw::com::GenericSkeleton> skeleton_;
    std::optional<score::mw::com::GenericProxy> proxy_;
    std::vector<score::mw::com::GenericProxyEvent*> subscribed_events_;
};

}

int main(int argc, char* argv[]) {
    try {
        Options options;
        if (const char* routes = std::getenv("SIGNAL_ROUTE_CONFIG")) {
            options.routes = routes;
        }
        for (int index = 1; index < argc; ++index) {
            const std::string_view argument{argv[index]};
            if (argument == "--help") {
                std::cout << "body_signal_test_app [--configuration PATH] [--routes PATH] "
                             "[--timeout-ms N] [--startup-timeout-ms N] [--list]\n";
                return EXIT_SUCCESS;
            }
            if (argument == "--list") {
                options.list = true;
            } else if (index + 1 < argc && (argument == "--configuration" || argument == "--routes")) {
                (argument == "--routes" ? options.routes : options.configuration) = argv[++index];
            } else if (index + 1 < argc &&
                       (argument == "--timeout-ms" || argument == "--startup-timeout-ms")) {
                const std::string value{argv[++index]};
                std::size_t parsed = 0;
                const long milliseconds = std::stol(value, &parsed);
                if (parsed != value.size() || milliseconds <= 0 || milliseconds > 60000) {
                    throw std::runtime_error("Timeout must be between 1 and 60000 milliseconds");
                }
                (argument == "--timeout-ms" ? options.timeout : options.startup_timeout) =
                    std::chrono::milliseconds{milliseconds};
            } else {
                throw std::runtime_error("Unknown or incomplete argument: " + std::string{argument});
            }
        }
        std::ifstream input{options.routes};
        const auto signals = body_signal_test::LoadSignals(nlohmann::json::parse(input));
        if (options.list) {
            for (const auto& signal : signals) {
                std::cout << signal.api_name << " [" << signal.datatype << "] " << signal.event_name << '\n';
            }
            std::cout << signals.size() << " signals\n";
            return EXIT_SUCCESS;
        }
        std::cout << "Body signal test: test rig only. This sends actuator values.\n";
        return SignalTestApplication{std::move(options), signals}.Run();
    } catch (const std::exception& error) {
        std::cerr << "Signal test error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}