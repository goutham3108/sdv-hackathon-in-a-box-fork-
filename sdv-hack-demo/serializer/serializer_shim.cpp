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

// Demo serializer plugin loaded by gatewayd in place of the gateway's own
// score_com_serializer.so. The gateway keys SOCom connectors by service type name, so a
// type with both a local and a remote instance collides inside gatewayd. The demo
// config therefore splits such a type into a Tx type and a "_rx" Rx type. The gateway
// serializer only knows the original names, so this shim maps the Rx name back to the
// original one and forwards every call to the gateway serializer, which is shipped next
// to this file as score_com_serializer_base.so.

#include <dlfcn.h>

#include <array>
#include <cstdio>
#include <string>
#include <string_view>
#include <utility>

#include "score/serializer/serializer.h"

namespace {

constexpr std::array<std::pair<std::string_view, std::string_view>, 1> kNameAliases{{
    {"vehicle::VehicleDynamicsService_rx", "vehicle::VehicleDynamicsService"},
}};

constexpr char kBaseLibraryName[] = "score_com_serializer_base.so";

struct BaseApi {
    decltype(&score_com_serializer_serialize) serialize{nullptr};
    decltype(&score_com_serializer_deserialize) deserialize{nullptr};
    decltype(&score_com_serializer_get_max_serialized_size) max_size{nullptr};
    decltype(&score_com_serializer_get_sizeof_type) size_of{nullptr};
    decltype(&score_com_serializer_get_alignof_type) align_of{nullptr};
    decltype(&score_com_serializer_get) get{nullptr};
    decltype(&score_com_serializer_init) init{nullptr};
    decltype(&score_com_serializer_deinit) deinit{nullptr};
    bool valid{false};
};

void* OpenBaseLibrary() {
    Dl_info self{};
    if (dladdr(reinterpret_cast<void*>(&OpenBaseLibrary), &self) != 0 && self.dli_fname != nullptr) {
        std::string path{self.dli_fname};
        const auto slash = path.find_last_of('/');
        if (slash != std::string::npos) {
            path.resize(slash + 1U);
            path += kBaseLibraryName;
            if (void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL)) {
                return handle;
            }
        }
    }
    return dlopen(kBaseLibraryName, RTLD_NOW | RTLD_LOCAL);
}

template <typename Fn>
bool Resolve(void* handle, const char* name, Fn& target) {
    target = reinterpret_cast<Fn>(dlsym(handle, name));
    return target != nullptr;
}

const BaseApi& Base() {
    static const BaseApi api = [] {
        BaseApi result{};
        void* const handle = OpenBaseLibrary();
        if (handle == nullptr) {
            std::fprintf(stderr, "[serializer shim] cannot load %s: %s\n", kBaseLibraryName,
                         dlerror());
            return result;
        }
        result.valid =
            Resolve(handle, "score_com_serializer_serialize", result.serialize) &&
            Resolve(handle, "score_com_serializer_deserialize", result.deserialize) &&
            Resolve(handle, "score_com_serializer_get_max_serialized_size", result.max_size) &&
            Resolve(handle, "score_com_serializer_get_sizeof_type", result.size_of) &&
            Resolve(handle, "score_com_serializer_get_alignof_type", result.align_of) &&
            Resolve(handle, "score_com_serializer_get", result.get) &&
            Resolve(handle, "score_com_serializer_init", result.init) &&
            Resolve(handle, "score_com_serializer_deinit", result.deinit);
        if (!result.valid) {
            std::fprintf(stderr, "[serializer shim] %s lacks required symbols\n", kBaseLibraryName);
        }
        return result;
    }();
    return api;
}

std::string_view CanonicalName(std::string_view name) {
    for (const auto& [alias, canonical] : kNameAliases) {
        if (name == alias) {
            return canonical;
        }
    }
    return name;
}

}  // namespace

extern "C" {

score_com_serializer_result score_com_serializer_serialize(
    const struct score_com_serializer* serializer, uint8_t* buffer, size_t buffer_size,
    const void* object, size_t* written_bytes) {
    const auto& base = Base();
    return base.valid ? base.serialize(serializer, buffer, buffer_size, object, written_bytes)
                      : score_com_serializer_result_general_failure;
}

score_com_serializer_result score_com_serializer_deserialize(
    const struct score_com_serializer* serializer, const uint8_t* buffer, size_t buffer_size,
    void* object) {
    const auto& base = Base();
    return base.valid ? base.deserialize(serializer, buffer, buffer_size, object)
                      : score_com_serializer_result_general_failure;
}

size_t score_com_serializer_get_max_serialized_size(const struct score_com_serializer* serializer) {
    const auto& base = Base();
    return base.valid ? base.max_size(serializer) : 0U;
}

size_t score_com_serializer_get_sizeof_type(const struct score_com_serializer* serializer) {
    const auto& base = Base();
    return base.valid ? base.size_of(serializer) : 0U;
}

size_t score_com_serializer_get_alignof_type(const struct score_com_serializer* serializer) {
    const auto& base = Base();
    return base.valid ? base.align_of(serializer) : 0U;
}

score_com_serializer_result score_com_serializer_get(
    const char* service_type, size_t service_type_size,
    enum score_com_serializer_element_type element_type, const char* element_name,
    size_t element_name_size, const struct score_com_serializer** serializer) {
    const auto& base = Base();
    if (!base.valid || service_type == nullptr) {
        return score_com_serializer_result_general_failure;
    }
    const std::string_view canonical = CanonicalName(std::string_view{service_type, service_type_size});
    return base.get(canonical.data(), canonical.size(), element_type, element_name,
                    element_name_size, serializer);
}

score_com_serializer_result score_com_serializer_init(const char* serializer_identifier,
                                                      size_t serializer_identifier_size) {
    const auto& base = Base();
    return base.valid ? base.init(serializer_identifier, serializer_identifier_size)
                      : score_com_serializer_result_general_failure;
}

score_com_serializer_result score_com_serializer_deinit() {
    const auto& base = Base();
    return base.valid ? base.deinit() : score_com_serializer_result_general_failure;
}

}  // extern "C"
