// Copyright (c) 2026 Contributors to the Eclipse Foundation
//
// SPDX-License-Identifier: Apache-2.0

// This file is a copy of the upstream remote application integration example.
// It is packaged here so the demo is self-contained under sdv-hack-demo.

#include <algorithm>
#include <atomic>
#include <array>
#include <cerrno>
#include <csignal>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <fcntl.h>
#include <linux/gpio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <nlohmann/json.hpp>
#include <vsomeip/vsomeip.hpp>

#include <kvsbuilder.hpp>
#include "high_beam_udp_protocol.h"
#include "sdv-hack-demo/include/speed_payload.h"
#include "score/mw/lifecycle/report_running.h"
#include "score/mw/log/logging.h"

namespace {

std::atomic<bool> running{true};
static_assert(std::atomic<bool>::is_always_lock_free);

void StopRemote(int) {
    running.store(false);
}

bool ReadInputLine(std::istream& stream, int fd, std::string& line) {
    while (running.load()) {
        pollfd descriptor{fd, POLLIN, 0};
        const int result = poll(&descriptor, 1, 100);
        if (result < 0 && errno != EINTR) {
            return false;
        }
        if (result > 0) {
            return static_cast<bool>(std::getline(stream, line));
        }
    }
    return false;
}

constexpr auto kPublishPeriod = std::chrono::seconds{2};
constexpr auto kBodySignalTestDelay = std::chrono::seconds{5};
constexpr auto kGpioSamplePeriod = std::chrono::milliseconds{5};
constexpr auto kGpioDebouncePeriod = std::chrono::milliseconds{30};
constexpr char kSensorStateKey[] = "high_beam_state";
constexpr std::uint16_t kDefaultBridgePort = 35000U;
constexpr std::uint16_t kDefaultRemotePort = 35001U;

struct Route {
    std::string name;
    std::string api_name;
    std::string datatype;
    std::uint16_t service;
    std::uint16_t vehicle_instance;
    std::uint16_t remote_instance;
    std::uint16_t vehicle_event;
    std::uint16_t remote_event;
};

bool PublishBooleanUpdate(const int socket_fd, const Route& route,
                          const sockaddr_in& destination, const bool value) {
    const auto frame = high_beam_udp::BooleanUpdate(
        route.service, route.remote_instance, route.remote_event, value);
    const auto encoded = high_beam_udp::Encode(frame);
    const auto sent = sendto(socket_fd, encoded.data(), encoded.size(), 0,
                             reinterpret_cast<const sockaddr*>(&destination), sizeof(destination));
    if (sent != static_cast<ssize_t>(encoded.size())) {
        score::mw::log::LogError() << "Remote failed to publish " << route.name
                                  << ": " << std::string{std::strerror(errno)};
        return false;
    }
    return true;
}

bool PublishAllBodySignals(const int socket_fd, const std::vector<Route>& routes,
                           const sockaddr_in& destination, std::mutex& high_beam_publish_mutex) {
    std::size_t published = 0;
    for (const auto& route : routes) {
        if (route.service != 17152U) {
            continue;
        }
        const auto values = high_beam_udp::TestPayloads(route.datatype, route.api_name);
        if (values.empty()) {
            score::mw::log::LogError() << "Unsupported test datatype for " << route.api_name;
            return false;
        }
        std::unique_lock<std::mutex> high_beam_test_lock{high_beam_publish_mutex, std::defer_lock};
        if (route.name == "high_beam") {
            // Hold off periodic high-beam frames until the complete test sequence is sent.
            high_beam_test_lock.lock();
        }
        for (const auto& value : values) {
            high_beam_udp::Frame frame{route.service, route.remote_instance, route.remote_event,
                                       static_cast<std::uint8_t>(value.size()), {}};
            std::copy(value.begin(), value.end(), frame.payload.begin());
            const auto encoded = high_beam_udp::Encode(frame);
            const auto sent = sendto(socket_fd, encoded.data(), encoded.size(), 0,
                                     reinterpret_cast<const sockaddr*>(&destination), sizeof(destination));
            if (sent != static_cast<ssize_t>(encoded.size())) {
                score::mw::log::LogError() << "Failed to publish test value for " << route.api_name
                                          << ": " << std::string{std::strerror(errno)};
                return false;
            }
            if (route.datatype == "boolean") {
                score::mw::log::LogWarn() << "Remote published test " << route.api_name
                                          << "=" << (value[0] == 0U ? "false" : "true");
            } else if (route.datatype == "uint8") {
                score::mw::log::LogWarn() << "Remote published test " << route.api_name
                                          << "=" << static_cast<unsigned int>(value[0]);
            } else if (route.datatype == "string") {
                score::mw::log::LogWarn() << "Remote published test " << route.api_name << "="
                                          << std::string{value.begin(), value.end()};
            } else {
                score::mw::log::LogWarn() << "Remote published test " << route.api_name
                                          << " payload_bytes=" << value.size();
            }
            ++published;
            std::this_thread::sleep_for(kBodySignalTestDelay);
        }
    }
    score::mw::log::LogWarn() << "Remote published " << published << " samples across 44 body signals";
    return true;
}

std::optional<bool> ParseBooleanInput(const std::string& input) {
    if (input == "true") {
        return true;
    }
    if (input == "false") {
        return false;
    }
    return std::nullopt;
}

std::optional<double> ParseSpeedInput(const std::string& input) {
    try {
        std::size_t parsed = 0U;
        const double value = std::stod(input, &parsed);
        if (parsed != input.size()) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

// Menu output goes to the same /dev/tty the input is read from, independent of stdout redirection.
int terminal_out_fd = -1;

void Say(const std::string& text) {
    if (terminal_out_fd >= 0) {
        std::size_t written = 0U;
        while (written < text.size()) {
            const ssize_t result = write(terminal_out_fd, text.data() + written, text.size() - written);
            if (result <= 0) {
                if (result < 0 && errno == EINTR) {
                    continue;
                }
                break;
            }
            written += static_cast<std::size_t>(result);
        }
        if (written == text.size()) {
            return;
        }
    }
    std::cout << text << std::flush;
}

void PrintRemoteMenu() {
    Say("\nRemote menu\n"
        "1  High-beam (true/false)\n"
        "2  Vehicle.speed (number)\n"
        "3  Vehicle.Body.Lights.Parking.IsOn (true/false)\n"
        "4  Publish test values for all 44 body signals\n"
        "q  Quit remote app\n"
        "Select: ");
}

std::uint16_t ResolvePort(const char* name, const std::uint16_t fallback) {
    const char* value = std::getenv(name);
    return value == nullptr ? fallback : static_cast<std::uint16_t>(std::stoi(value));
}

std::string DetectGpioChip() {
    for (unsigned int index = 0U; index < 64U; ++index) {
        const std::string path = "/dev/gpiochip" + std::to_string(index);
        const int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
        if (fd < 0) {
            continue;
        }

        gpiochip_info info{};
        const bool found = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &info) == 0 &&
                   (std::strncmp(info.name, "pinctrl-", 8U) == 0 ||
                    std::strncmp(info.label, "pinctrl-", 8U) == 0);
        close(fd);
        if (found) {
            return path;
        }
    }
    return "/dev/gpiochip0";
}

int OpenGpioInput() {
    const char* const configured_chip = std::getenv("HIGH_BEAM_GPIO_CHIP");
    const std::string chip_path = configured_chip == nullptr || configured_chip[0] == '\0'
                                      ? DetectGpioChip()
                                      : configured_chip;
    const char* const configured_line = std::getenv("HIGH_BEAM_GPIO_LINE");
    unsigned long line = 17UL;
    if (configured_line != nullptr && configured_line[0] != '\0') {
        char* end = nullptr;
        errno = 0;
        line = std::strtoul(configured_line, &end, 10);
        if (errno != 0 || end == configured_line || *end != '\0' || line > UINT32_MAX) {
            score::mw::log::LogError() << "Invalid HIGH_BEAM_GPIO_LINE: " << configured_line;
            return -1;
        }
    }

    const int chip_fd = open(chip_path.c_str(), O_RDONLY | O_CLOEXEC);
    if (chip_fd < 0) {
        score::mw::log::LogError() << "Cannot open GPIO chip " << chip_path << ": "
                                  << std::strerror(errno);
        return -1;
    }

    gpiohandle_request request{};
    request.lineoffsets[0] = static_cast<std::uint32_t>(line);
    request.flags = GPIOHANDLE_REQUEST_INPUT | GPIOHANDLE_REQUEST_BIAS_PULL_UP;
    request.lines = 1U;
    std::strncpy(request.consumer_label, "sdv-remote-highbeam", sizeof(request.consumer_label) - 1U);
    const int result = ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &request);
    const int request_error = errno;
    close(chip_fd);
    if (result < 0) {
        score::mw::log::LogError() << "Cannot request BCM GPIO " << line << " with pull-up: "
                                  << std::strerror(request_error);
        return -1;
    }
    score::mw::log::LogWarn() << "Using GPIO controller " << chip_path << " line " << line;
    return request.fd;
}

bool ReadGpioHigh(const int gpio_fd, bool& high) {
    gpiohandle_data values{};
    if (ioctl(gpio_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &values) < 0) {
        score::mw::log::LogError() << "Cannot read high-beam GPIO: " << std::strerror(errno);
        return false;
    }
    high = values.values[0] != 0U;
    return true;
}

class GpioOutput {
 public:
    ~GpioOutput() {
        if (fd_ >= 0) {
            (void)Write(false);
            close(fd_);
        }
    }

    bool Open(const bool initial_value) {
        const char* configured_chip = std::getenv("HIGH_BEAM_GPIO_OUTPUT_CHIP");
        const char* input_chip = std::getenv("HIGH_BEAM_GPIO_CHIP");
        const std::string input_path = input_chip == nullptr || input_chip[0] == '\0'
            ? DetectGpioChip() : input_chip;
        const std::string chip = configured_chip == nullptr || configured_chip[0] == '\0'
            ? input_path : configured_chip;
        try {
            const char* configured_line = std::getenv("HIGH_BEAM_GPIO_OUTPUT_LINE");
            const std::string text = configured_line == nullptr ? "27" : configured_line;
            std::size_t parsed = 0;
            const auto line = std::stoul(text, &parsed);
            if (parsed != text.size() || line > UINT32_MAX || text.front() == '-') {
                throw std::invalid_argument("invalid output line");
            }
            line_ = static_cast<std::uint32_t>(line);
            const char* input_line = std::getenv("HIGH_BEAM_GPIO_LINE");
            const auto input_offset = input_line == nullptr ? 17UL : std::stoul(input_line);
            if (chip == input_path && line_ == input_offset) {
                score::mw::log::LogError() << "GPIO input and output must use different lines";
                return false;
            }
        } catch (const std::exception& error) {
            score::mw::log::LogError() << "Invalid GPIO output configuration: " << std::string{error.what()};
            return false;
        }
        const int chip_fd = open(chip.c_str(), O_RDONLY | O_CLOEXEC);
        if (chip_fd < 0) {
            score::mw::log::LogError() << "Cannot open GPIO output chip " << chip;
            return false;
        }
        gpiohandle_request request{};
        request.lineoffsets[0] = line_;
        request.flags = GPIOHANDLE_REQUEST_OUTPUT;
        request.default_values[0] = initial_value ? 1U : 0U;
        request.lines = 1;
        std::strncpy(request.consumer_label, "sdv-highbeam-output", sizeof(request.consumer_label) - 1);
        const int result = ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &request);
        const int error = errno;
        close(chip_fd);
        if (result < 0) {
            score::mw::log::LogError() << "Cannot request GPIO output: " << std::string{std::strerror(error)};
            return false;
        }
        fd_ = request.fd;
        last_logged_state_ = initial_value;
        score::mw::log::LogWarn() << "Remote LED initialized " << (initial_value ? "ON" : "OFF")
                      << ": " << chip << " line " << line_;
        return true;
    }

    bool Write(bool value) {
        gpiohandle_data data{};
        data.values[0] = value ? 1 : 0;
        if (ioctl(fd_, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
            score::mw::log::LogError() << "GPIO output write failed: " << std::string{std::strerror(errno)};
            return false;
        }
        if (!last_logged_state_.has_value() || last_logged_state_.value() != value) {
            score::mw::log::LogWarn() << "Remote GPIO output applied High.IsOn=" << value << " line " << line_;
            last_logged_state_ = value;
        }
        return true;
    }

 private:
    int fd_{-1};
    std::uint32_t line_{27};
    std::optional<bool> last_logged_state_;
};

// Single entry point for every High.IsOn source; does nothing unless the value changed.
void SetOutboundState(const bool value, GpioOutput* const led, std::atomic<bool>& outbound_state,
                      const std::shared_ptr<score::mw::per::kvs::Kvs>& sensor_store,
                      const char* source, std::condition_variable& outbound_changed,
                      std::mutex& outbound_mutex, std::uint64_t& outbound_generation) {
    {
        std::lock_guard<std::mutex> lock{outbound_mutex};
        if (outbound_state.exchange(value) == value) {
            return;
        }
        (void)sensor_store->set_value(kSensorStateKey, score::mw::per::kvs::KvsValue{value});
        (void)sensor_store->flush();
        score::mw::log::LogWarn() << "Remote " << source << " set High.IsOn="
                                  << (value ? "true" : "false");
        if (led != nullptr && !led->Write(value)) {
            running.store(false);
            return;
        }
        ++outbound_generation;
    }
    outbound_changed.notify_one();
}

void MonitorGpio(const int gpio_fd, GpioOutput* const led, std::atomic<bool>& outbound_state,
                 const std::shared_ptr<score::mw::per::kvs::Kvs>& sensor_store,
                 std::condition_variable& outbound_changed, std::mutex& outbound_mutex,
                 std::uint64_t& outbound_generation) {
    bool line_high = true;
    if (!ReadGpioHigh(gpio_fd, line_high)) {
        close(gpio_fd);
        running.store(false);
        return;
    }

    bool candidate_pressed = !line_high;
    bool stable_pressed = candidate_pressed;
    auto candidate_since = std::chrono::steady_clock::now();
    // An idle switch at startup must not overwrite the restored state.
    if (stable_pressed) {
        SetOutboundState(stable_pressed, led, outbound_state, sensor_store, "GPIO BCM 17",
                         outbound_changed, outbound_mutex, outbound_generation);
    }
    score::mw::log::LogWarn() << "Remote GPIO input active on BCM 17 (physical pin 11); "
                              << "switch connects GPIO to GND when pressed";

    while (running.load()) {
        std::this_thread::sleep_for(kGpioSamplePeriod);
        if (!ReadGpioHigh(gpio_fd, line_high)) {
            running.store(false);
            break;
        }
        const bool pressed = !line_high;
        if (pressed != candidate_pressed) {
            candidate_pressed = pressed;
            candidate_since = std::chrono::steady_clock::now();
        } else if (candidate_pressed != stable_pressed &&
                   std::chrono::steady_clock::now() - candidate_since >= kGpioDebouncePeriod) {
            stable_pressed = candidate_pressed;
            SetOutboundState(stable_pressed, led, outbound_state, sensor_store, "GPIO BCM 17",
                             outbound_changed, outbound_mutex, outbound_generation);
        }
    }
    close(gpio_fd);
}

std::string ResolveAddress(const char* name, const char* fallback) {
    const char* value = std::getenv(name);
    return value == nullptr || value[0] == '\0' ? fallback : value;
}

bool SetAddress(sockaddr_in& address, const std::string& value) {
    return inet_pton(AF_INET, value.c_str(), &address.sin_addr) == 1;
}

std::optional<std::vector<Route>> LoadRoutes() {
    const char* const configured = std::getenv("SIGNAL_ROUTE_CONFIG");
    const std::string path = configured == nullptr || configured[0] == '\0' ? "signal_routes.json" : configured;
    std::ifstream input{path};
    if (!input.is_open()) {
        score::mw::log::LogError() << "Cannot open signal route configuration";
        return std::nullopt;
    }
    try {
        const auto document = nlohmann::json::parse(input);
        std::vector<Route> routes;
        for (const auto& item : document.at("routes")) {
            const auto name = item.at("name").get<std::string>();
            routes.push_back(Route{name, item.value("apiName", name), item.value("datatype", std::string{}),
                                   item.at("service").get<std::uint16_t>(),
                                   item.at("vehicleInstance").get<std::uint16_t>(),
                                   item.at("remoteInstance").get<std::uint16_t>(),
                                   item.at("vehicleEvent").get<std::uint16_t>(),
                                   item.at("remoteEvent").get<std::uint16_t>()});
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
    std::signal(SIGINT, StopRemote);
    std::signal(SIGTERM, StopRemote);
    const auto routes = LoadRoutes();
    if (!routes.has_value() || routes->empty()) {
        return 1;
    }
    const auto high_beam = std::find_if(routes->begin(), routes->end(), [](const Route& route) {
        return route.name == "high_beam";
    });
    if (high_beam == routes->end()) {
        score::mw::log::LogError() << "Route configuration has no high_beam route";
        return 1;
    }
    const auto parking_lights = std::find_if(routes->begin(), routes->end(), [](const Route& route) {
        return route.name == "parking_lights";
    });
    std::atomic<int> parking_state{-1};
    const char* const configured_kvs_dir = std::getenv("SDV_KVS_DIR");
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
    bool initial_output = false;
    const auto stored_command = sensor_store->get_value(kSensorStateKey);
    if (stored_command.has_value() &&
        stored_command.value().getType() == score::mw::per::kvs::KvsValue::Type::Boolean) {
        initial_output = std::get<bool>(stored_command.value().getValue());
    }
    std::atomic<bool> outbound_state{initial_output};
    std::condition_variable outbound_changed;
    std::mutex outbound_mutex;
    std::mutex high_beam_publish_mutex;
    std::uint64_t outbound_generation{0U};

    const std::uint16_t bridge_port = ResolvePort("SDV_BRIDGE_UDP_PORT", kDefaultBridgePort);
    const std::uint16_t remote_port = ResolvePort("SDV_REMOTE_UDP_PORT", kDefaultRemotePort);
    const std::string bind_ip = ResolveAddress("SDV_BIND_IP", "0.0.0.0");
    const std::string bridge_ip = ResolveAddress("SDV_BRIDGE_IP", "127.0.0.1");
    const int udp_socket = CreateUdpSocket(remote_port, bind_ip);
    if (udp_socket < 0) {
        score::mw::log::LogError() << "Failed to bind remote UDP port " << remote_port;
        return 1;
    }
    timeval receive_timeout{0, 200000};
    if (setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO, &receive_timeout, sizeof(receive_timeout)) < 0) {
        close(udp_socket);
        return 1;
    }
    sockaddr_in bridge_address{};
    bridge_address.sin_family = AF_INET;
    if (!SetAddress(bridge_address, bridge_ip)) {
        score::mw::log::LogError() << "Invalid SDV_BRIDGE_IP: " << bridge_ip;
        close(udp_socket);
        return 1;
    }
    bridge_address.sin_port = htons(bridge_port);
    if (std::getenv("PROCESSIDENTIFIER") != nullptr) {
        score::mw::lifecycle::report_running();
    }

    std::thread gpio_thread;
    GpioOutput led;
    const bool output_available = led.Open(initial_output);
    GpioOutput* const led_output = output_available ? &led : nullptr;
    // GPIO failures are logged but not fatal so the menu and vehicle commands keep working off-Pi.
    const int gpio_fd = OpenGpioInput();
    if (gpio_fd >= 0) {
        gpio_thread = std::thread([gpio_fd, led_output, &outbound_state, &sensor_store, &outbound_changed,
                                    &outbound_mutex, &outbound_generation]() {
            MonitorGpio(gpio_fd, led_output, outbound_state, sensor_store, outbound_changed, outbound_mutex,
                        outbound_generation);
        });
    }
    std::thread input_thread([led_output, &outbound_state, &sensor_store, &routes, &bridge_address, udp_socket,
                              &outbound_changed, &outbound_mutex, &outbound_generation,
                              &high_beam_publish_mutex,
                              parking_lights, &parking_state]() {
            std::ifstream terminal_input{"/dev/tty"};
            const int terminal_fd = terminal_input.is_open() ? open("/dev/tty", O_RDONLY | O_CLOEXEC) : -1;
            const int input_fd = terminal_fd >= 0 ? terminal_fd : STDIN_FILENO;
            terminal_out_fd = open("/dev/tty", O_WRONLY | O_CLOEXEC | O_NOCTTY);
            std::istream& input_stream = terminal_input.is_open()
                                             ? static_cast<std::istream&>(terminal_input)
                                             : std::cin;
            PrintRemoteMenu();
            std::string input;
            while (ReadInputLine(input_stream, input_fd, input)) {
                if (input == "q") {
                    running.store(false);
                    break;
                }
                if (input == "1") {
                    Say("High-beam value (true/false): ");
                    if (!ReadInputLine(input_stream, input_fd, input)) {
                        break;
                    }
                    const auto value = ParseBooleanInput(input);
                    if (!value.has_value()) {
                        score::mw::log::LogWarn() << "Invalid high-beam input. Enter true or false.";
                        continue;
                    }
                    SetOutboundState(value.value(), led_output, outbound_state, sensor_store, "menu",
                                     outbound_changed, outbound_mutex, outbound_generation);
                    PrintRemoteMenu();
                    continue;
                }
                if (input == "2") {
                    const auto dynamics = std::find_if(routes->begin(), routes->end(), [](const Route& route) {
                        return route.name == "vehicle_dynamics";
                    });
                    if (dynamics == routes->end()) {
                        score::mw::log::LogError() << "Route configuration has no vehicle_dynamics route";
                        continue;
                    }
                    Say("Vehicle.speed value: ");
                    if (!ReadInputLine(input_stream, input_fd, input)) {
                        break;
                    }
                    const auto speed = ParseSpeedInput(input);
                    if (!speed.has_value()) {
                        score::mw::log::LogWarn() << "Invalid speed input. Enter a number.";
                        continue;
                    }
                    high_beam_udp::Frame frame{dynamics->service, dynamics->remote_instance,
                                               dynamics->remote_event, 9U, {}};
                    vehicle_dynamics_wire::EncodeSpeed(speed.value(), 3U, frame.payload.data());
                    const auto encoded = high_beam_udp::Encode(frame);
                    (void)sendto(udp_socket, encoded.data(), encoded.size(), 0,
                                 reinterpret_cast<const sockaddr*>(&bridge_address), sizeof(bridge_address));
                    score::mw::log::LogWarn() << "Remote published Vehicle.speed=" << speed.value();
                    PrintRemoteMenu();
                    continue;
                }
                if (input == "3") {
                    if (parking_lights == routes->end()) {
                        score::mw::log::LogError() << "Route configuration has no parking_lights route";
                        PrintRemoteMenu();
                        continue;
                    }
                    Say("Vehicle.Body.Lights.Parking.IsOn value (true/false): ");
                    if (!ReadInputLine(input_stream, input_fd, input)) {
                        break;
                    }
                    const auto value = ParseBooleanInput(input);
                    if (!value.has_value()) {
                        score::mw::log::LogWarn() << "Invalid parking-light input. Enter true or false.";
                        PrintRemoteMenu();
                        continue;
                    }
                    parking_state.store(value.value() ? 1 : 0);
                    if (PublishBooleanUpdate(udp_socket, *parking_lights, bridge_address, value.value())) {
                        score::mw::log::LogWarn() << "Remote published Vehicle.Body.Lights.Parking.IsOn="
                                                  << value.value();
                    }
                    PrintRemoteMenu();
                    continue;
                }
                if (input == "4") {
                    Say("Publishing test values for all configured body signals...\n");
                    (void)PublishAllBodySignals(udp_socket, *routes, bridge_address,
                                                high_beam_publish_mutex);
                    PrintRemoteMenu();
                    continue;
                }
                score::mw::log::LogWarn() << "Invalid menu option. Select 1, 2, 3, 4, or q.";
            }
            if (terminal_fd >= 0) {
                close(terminal_fd);
            }
            if (terminal_out_fd >= 0) {
                close(terminal_out_fd);
                terminal_out_fd = -1;
            }
        });
    std::thread receive_thread([udp_socket, &routes, &bridge_address, led_output, &sensor_store,
                                &outbound_state, &outbound_changed, &outbound_mutex,
                                &outbound_generation, &parking_state]() {
        std::array<std::uint8_t, high_beam_udp::kFrameSize> bytes{};
        std::optional<double> last_logged_speed;
        while (running.load()) {
            sockaddr_in sender{};
            socklen_t sender_size = sizeof(sender);
            const auto received = recvfrom(udp_socket, bytes.data(), bytes.size(), 0,
                                           reinterpret_cast<sockaddr*>(&sender), &sender_size);
            if (received <= 0) {
                continue;
            }
            const auto frame = high_beam_udp::Decode(bytes.data(), static_cast<std::size_t>(received));
            const auto route = frame.has_value()
                                   ? std::find_if(routes->begin(), routes->end(), [&frame](const Route& candidate) {
                                         return frame->service == candidate.service &&
                                                frame->instance == candidate.vehicle_instance &&
                                                frame->event == candidate.vehicle_event;
                                     })
                                   : routes->end();
            if (!frame.has_value() || route == routes->end()) {
                score::mw::log::LogError() << "Remote app received invalid UDP SOME/IP frame";
                continue;
            }
            char sender_ip[INET_ADDRSTRLEN]{};
            const char* const sender_address =
                inet_ntop(AF_INET, &sender.sin_addr, sender_ip, sizeof(sender_ip));
            if (route->name != "high_beam") {
                if (route->name == "parking_lights") {
                    if (frame->payload_size != 1U || frame->payload[0] > 1U) {
                        score::mw::log::LogError() << "Remote rejected malformed parking-light command";
                        continue;
                    }
                    parking_state.store(frame->payload[0]);
                    score::mw::log::LogWarn() << "Remote received Vehicle.Body.Lights.Parking.IsOn="
                                              << (frame->payload[0] != 0U);
                }
                if (frame->payload_size == sizeof(double) + 1U) {
                    const double received_value = vehicle_dynamics_wire::DecodeSpeed(frame->payload.data());
                    if (!last_logged_speed.has_value() || last_logged_speed.value() != received_value) {
                        score::mw::log::LogWarn() << "Remote received Vehicle.speed=" << received_value
                                              << " from "
                                              << (sender_address == nullptr ? "<unknown>" : sender_address)
                                              << ":" << ntohs(sender.sin_port);
                        last_logged_speed = received_value;
                    }
                } else {
                    score::mw::log::LogWarn() << "Remote received " << route->name
                                              << " payload_size=" << frame->payload_size;
                }
                high_beam_udp::Frame acknowledgement{route->service, route->remote_instance,
                                                      route->remote_event, frame->payload_size, frame->payload};
                const auto encoded = high_beam_udp::Encode(acknowledgement);
                (void)sendto(udp_socket, encoded.data(), encoded.size(), 0,
                             reinterpret_cast<const sockaddr*>(&bridge_address), sizeof(bridge_address));
                if (frame->payload_size != sizeof(double) + 1U) {
                    score::mw::log::LogWarn() << "Remote acknowledged " << route->name
                                              << " payload_size=" << frame->payload_size;
                }
                continue;
            }
            if (frame->payload_size != 1U || frame->payload[0] > 1U) {
                score::mw::log::LogError() << "Remote rejected malformed high-beam command";
                continue;
            }
            SetOutboundState(frame->payload[0] == 1U, led_output, outbound_state, sensor_store,
                             "vehicle command", outbound_changed, outbound_mutex, outbound_generation);
        }
    });

    std::uint64_t last_sent_generation = std::numeric_limits<std::uint64_t>::max();
    auto next_publish = std::chrono::steady_clock::now();
    while (running.load()) {
        {
            std::unique_lock<std::mutex> lock{outbound_mutex};
            outbound_changed.wait_for(lock, std::chrono::milliseconds{100}, [&]() {
                return outbound_generation != last_sent_generation;
            });
            if (!running.load()) {
                break;
            }
            if (outbound_generation == last_sent_generation && std::chrono::steady_clock::now() < next_publish) {
                continue;
            }
            last_sent_generation = outbound_generation;
        }
        std::lock_guard<std::mutex> publish_lock{high_beam_publish_mutex};
        const bool value = outbound_state.load();
        high_beam_udp::Frame outbound{high_beam->service, high_beam->remote_instance,
                          high_beam->remote_event, 1U,
                          {static_cast<std::uint8_t>(value)}};
        const auto frame = high_beam_udp::Encode(outbound);
        (void)sendto(udp_socket, frame.data(), frame.size(), 0,
                     reinterpret_cast<const sockaddr*>(&bridge_address), sizeof(bridge_address));
        const int parking_value = parking_state.load();
        if (parking_lights != routes->end() && parking_value >= 0) {
            (void)PublishBooleanUpdate(udp_socket, *parking_lights, bridge_address, parking_value != 0);
        }
        next_publish = std::chrono::steady_clock::now() + kPublishPeriod;
    }
    input_thread.join();
    if (gpio_thread.joinable()) {
        gpio_thread.join();
    }
    receive_thread.join();
    close(udp_socket);
    return 0;
}
