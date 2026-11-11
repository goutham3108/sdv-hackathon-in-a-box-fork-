/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

/// @file
/// This file provides a "serializer" which actually doesn't serialize and just copies the memory.

#include <csignal>
#include <cstddef>
#include <cstring>
#include <limits>
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

#include "score/config/mw_someip_config_generated.h"
#include "score/mw/log/logging.h"
#include "score/serializer/pre_serialized_data.h"
#include "score/serializer/serializer.h"

using score::someip_gateway::serializer::get_size_of_pre_serialized_data;
using score::someip_gateway::serializer::PreSerializedData;

// We don't know the actual size of the PreSerializedData struct at compile time because it depends
// on the max_message_size specified in the config, so we use a view with zero-sized array and
// calculate the size at runtime.
using PreSerializedDataView = PreSerializedData<0>;

// score_com_serializer is an opaque handle that directly points to
// score::mw_someip_config::NullSerializerConfig in the flatbuffer config.
struct score_com_serializer {};

namespace {

struct VehicleDynamicsSample {
    double value;
    std::uint8_t quality;
};

static_assert(sizeof(double) == 8 && std::numeric_limits<double>::is_iec559);
static_assert(offsetof(VehicleDynamicsSample, quality) == 8);
constexpr std::size_t kVehicleDynamicsWireSize = 9;
const score_com_serializer kVehicleDynamicsSerializer{};
const score_com_serializer kBooleanSerializer{};

/// Convert from opaque score_com_serializer pointer to NullSerializerConfig
const score::mw_someip_config::NullSerializerConfig* to_null_config(
    const struct score_com_serializer* serializer) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<const score::mw_someip_config::NullSerializerConfig*>(serializer);
}

std::shared_ptr<const score::mw_someip_config::Root>& get_config() {
    static std::shared_ptr<const score::mw_someip_config::Root> config;
    return config;
}

};  // anonymous namespace

score_com_serializer_result score_com_serializer_serialize(const struct score_com_serializer* serializer,
                                                           uint8_t* buffer, size_t buffer_size,
                                                           const void* object,
                                                           size_t* written_bytes) {
    if (buffer == nullptr || object == nullptr) {
        return score_com_serializer_result_general_failure;
    }
    if (serializer == &kVehicleDynamicsSerializer) {
        if (buffer_size < kVehicleDynamicsWireSize) {
            return score_com_serializer_result_serialization_failure;
        }
        std::uint64_t bits{};
        std::memcpy(&bits, object, sizeof(bits));
        for (std::size_t index = 0; index < sizeof(bits); ++index) {
            buffer[index] = static_cast<std::uint8_t>(bits >> ((7U - index) * 8U));
        }
        std::memcpy(buffer + 8, static_cast<const std::uint8_t*>(object) +
                                   offsetof(VehicleDynamicsSample, quality), 1);
        if (written_bytes != nullptr) {
            *written_bytes = kVehicleDynamicsWireSize;
        }
        return score_com_serializer_result_ok;
    }
    if (serializer == &kBooleanSerializer) {
        if (buffer_size < 1U) {
            return score_com_serializer_result_serialization_failure;
        }
        buffer[0] = *static_cast<const bool*>(object) ? 1U : 0U;
        if (written_bytes != nullptr) {
            *written_bytes = 1U;
        }
        return score_com_serializer_result_ok;
    }
    const auto* pre_serialized_data = static_cast<const PreSerializedDataView*>(object);
    std::size_t message_size = pre_serialized_data->size;
    if (message_size > buffer_size) {
        return score_com_serializer_result_serialization_failure;
    }
    std::memcpy(buffer, pre_serialized_data->data, message_size);
    if (written_bytes != nullptr) {
        *written_bytes = message_size;
    }
    return score_com_serializer_result_ok;
}

score_com_serializer_result score_com_serializer_deserialize(
    const struct score_com_serializer* serializer, const uint8_t* buffer, size_t buffer_size,
    void* object) {
    if (serializer == nullptr || buffer == nullptr || object == nullptr) {
        return score_com_serializer_result_general_failure;
    }
    if (serializer == &kVehicleDynamicsSerializer) {
        if (buffer_size != kVehicleDynamicsWireSize) {
            return score_com_serializer_result_deserialization_failure;
        }
        std::uint64_t bits{};
        for (std::size_t index = 0; index < sizeof(bits); ++index) {
            bits = (bits << 8U) | buffer[index];
        }
        std::memset(object, 0, sizeof(VehicleDynamicsSample));
        std::memcpy(object, &bits, sizeof(bits));
        std::memcpy(static_cast<std::uint8_t*>(object) +
                        offsetof(VehicleDynamicsSample, quality), buffer + 8, 1);
        return score_com_serializer_result_ok;
    }
    if (serializer == &kBooleanSerializer) {
        if (buffer_size != 1U || buffer[0] > 1U) {
            return score_com_serializer_result_deserialization_failure;
        }
        *static_cast<bool*>(object) = buffer[0] != 0U;
        return score_com_serializer_result_ok;
    }
    auto* pre_serialized_data = static_cast<PreSerializedDataView*>(object);
    if (buffer_size > to_null_config(serializer)->max_message_size()) {
        return score_com_serializer_result_deserialization_failure;
    }
    std::memcpy(pre_serialized_data->data, buffer, buffer_size);
    pre_serialized_data->size = buffer_size;
    return score_com_serializer_result_ok;
}

std::size_t score_com_serializer_get_max_serialized_size(
    const struct score_com_serializer* serializer) {
    if (serializer == &kVehicleDynamicsSerializer) {
        return kVehicleDynamicsWireSize;
    }
    if (serializer == &kBooleanSerializer) {
        return 1U;
    }
    if (serializer == nullptr) {
        return 0;
    }
    return to_null_config(serializer)->max_message_size();
}

std::size_t score_com_serializer_get_sizeof_type(const struct score_com_serializer* serializer) {
    if (serializer == &kVehicleDynamicsSerializer) {
        return sizeof(VehicleDynamicsSample);
    }
    if (serializer == &kBooleanSerializer) {
        return sizeof(bool);
    }
    if (serializer == nullptr) {
        return 0;
    }
    return get_size_of_pre_serialized_data(to_null_config(serializer)->max_message_size());
}

std::size_t score_com_serializer_get_alignof_type(const struct score_com_serializer* serializer) {
    if (serializer == &kVehicleDynamicsSerializer) {
        return alignof(VehicleDynamicsSample);
    }
    if (serializer == &kBooleanSerializer) {
        return alignof(bool);
    }
    return alignof(PreSerializedDataView);
}

score_com_serializer_result score_com_serializer_init(const char* serializer_identifier,
                                                      size_t serializer_identifier_size) {
    std::string_view serializer_id(serializer_identifier, serializer_identifier_size);

    // Read config data
    // TODO: Use memory mapped file instead of copying into buffer
    std::ifstream config_file;
    config_file.open(std::string(serializer_id), std::ios::binary | std::ios::in);

    if (!config_file.is_open()) {
        score::mw::log::LogError() << "Error: Could not open config file " << serializer_id;
        return score_com_serializer_result_serializer_nonexistent;
    }

    config_file.seekg(0, std::ios::end);
    std::streampos length = config_file.tellg();

    if (length <= 0) {
        score::mw::log::LogError()
            << "Error: Invalid config file size: " << static_cast<std::size_t>(length);
        config_file.close();
        return score_com_serializer_result_serializer_nonexistent;
    }

    config_file.seekg(0, std::ios::beg);
    auto config_buffer = std::shared_ptr<char[]>(new char[length]);
    config_file.read(config_buffer.get(), length);
    config_file.close();

    get_config() = std::shared_ptr<const score::mw_someip_config::Root>(
        config_buffer, score::mw_someip_config::GetRoot(config_buffer.get()));

    return score_com_serializer_result_ok;
}

score_com_serializer_result score_com_serializer_deinit() {
    get_config().reset();
    return score_com_serializer_result_ok;
}

namespace {

const score::mw_someip_config::NullSerializerConfig* lookup_serialization_config(
    std::string_view service_type_name, score_com_serializer_element_type element_type,
    std::string_view element_name) {
    const auto config = get_config();
    if (config == nullptr || config->service_types() == nullptr) {
        return nullptr;
    }

    for (const auto* service_type : *config->service_types()) {
        if (service_type->service_type_name() == nullptr) {
            continue;
        }
        const auto configured_service_type_name = service_type->service_type_name()->string_view();
        const bool is_high_beam_name_alias =
            service_type_name == "/body_control_module_rx" &&
            configured_service_type_name == "body_control_module_rx";
        if (configured_service_type_name != service_type_name && !is_high_beam_name_alias) {
            continue;
        }

        if (element_type == score_com_serializer_element_type_event) {
            if (service_type->events() == nullptr) {
                return nullptr;
            }
            for (const auto* event : *service_type->events()) {
                if (event->event_name() != nullptr &&
                    event->event_name()->string_view() == element_name) {
                    if (event->serialization_config_as_VehicleDynamicsSerializerConfig() != nullptr) {
                        if (service_type_name == "vehicle::VehicleDynamicsService" &&
                            (element_name == "speed" || element_name == "speedAck")) {
                            // The caller turns the result back into a score_com_serializer*.
                            return reinterpret_cast<const score::mw_someip_config::NullSerializerConfig*>(
                                &kVehicleDynamicsSerializer);
                        }
                        return nullptr;
                    }
                    if (event->serialization_config_as_BooleanSerializerConfig() != nullptr) {
                        return reinterpret_cast<const score::mw_someip_config::NullSerializerConfig*>(
                            &kBooleanSerializer);
                    }
                    return event->serialization_config_as_NullSerializerConfig();
                }
            }
        } else if (element_type == score_com_serializer_element_type_method_call) {
            if (service_type->methods() == nullptr) {
                return nullptr;
            }
            for (const auto* method : *service_type->methods()) {
                if (method->method_name() != nullptr &&
                    method->method_name()->string_view() == element_name) {
                    return method->request_serialization_config_as_NullSerializerConfig();
                }
            }
        } else if (element_type == score_com_serializer_element_type_method_response) {
            if (service_type->methods() == nullptr) {
                return nullptr;
            }
            for (const auto* method : *service_type->methods()) {
                if (method->method_name() != nullptr &&
                    method->method_name()->string_view() == element_name) {
                    return method->response_serialization_config_as_NullSerializerConfig();
                }
            }
        }

        return nullptr;
    }

    return nullptr;
}

}  // namespace

score_com_serializer_result score_com_serializer_get(
    const char* service_type, size_t service_type_size,
    enum score_com_serializer_element_type element_type, const char* element_name,
    size_t element_name_size, const struct score_com_serializer** serializer) {
    if (serializer == nullptr) {
        return score_com_serializer_result_general_failure;
    }

    std::string_view service_type_name(service_type, service_type_size);
    std::string_view element_name_view(element_name, element_name_size);

    const auto* serializer_config =
        lookup_serialization_config(service_type_name, element_type, element_name_view);
    if (serializer_config == nullptr) {
        score::mw::log::LogError()
            << "Error: No serialization config found for service_type=" << service_type_name
            << " element=" << element_name_view;
        return score_com_serializer_result_serializer_nonexistent;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    *serializer = reinterpret_cast<const struct score_com_serializer*>(serializer_config);

    return score_com_serializer_result_ok;
}
