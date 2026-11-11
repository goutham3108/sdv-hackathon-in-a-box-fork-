# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_rust//rust:defs.bzl", "rust_library")
load("@score_baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")
load("//quality/unit_testing:unit_testing.bzl", "cc_unit_test")

rust_library(
    name = "com-api-gen",
    srcs = ["com_api_gen.rs"],
    crate_name = "com_api_gen",
    features = ["link_std_cpp_lib"],
    visibility = [
        "//score/mw/com:__subpackages__",
    ],
    deps = [
        ":vehicle_gen_cpp",
        "//score/mw/com/rust:score_com",
        "@score_baselibs//score/log_rust/score_log",
    ],
)

cc_library(
    name = "vehicle_gen_types",
    hdrs = [
        "body_signal_events.inc",
        "vehicle_gen.h",
    ],
    visibility = ["//score/mw/com:__subpackages__"],
    deps = ["//score/mw/com:types"],
)

cc_library(
    name = "vehicle_gen_cpp",
    srcs = ["vehicle_gen.cpp"],
    features = COMPILER_WARNING_FEATURES,
    implementation_deps = [
        "//score/mw/com/rust/score_com_cpp_bridge:register_interface",
    ],
    alwayslink = True,
    deps = [":vehicle_gen_types"],
)

cc_unit_test(
    name = "body_signal_interface_test",
    srcs = ["body_signal_interface_test.cpp"],
    features = COMPILER_WARNING_FEATURES,
    deps = [
        ":vehicle_gen_types",
        "@googletest//:gtest",
    ],
)
