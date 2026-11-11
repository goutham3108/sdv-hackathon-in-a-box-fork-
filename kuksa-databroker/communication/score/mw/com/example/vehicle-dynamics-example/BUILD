# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

load("@rules_rust//rust:defs.bzl", "rust_binary", "rust_library", "rust_test")

rust_library(
    name = "vehicle-dynamics-example-lib",
    srcs = glob(["src/**/*.rs"]),
    crate_name = "vehicle_dynamics_example",
    edition = "2024",
    features = ["link_std_cpp_lib"],
    deps = [
        "//score/mw/com/example/vehicle-dynamics-example/com-api-gen",
        "//score/mw/com/rust:score_com",
        "@score_baselibs//score/log_rust/score_log",
        "@score_communication_crate_index//:clap",
        "@score_communication_crate_index//:futures",
    ],
)

rust_binary(
    name = "vehicle-dynamics-example",
    srcs = ["main.rs"],
    data = [
        "etc/logging.json",
        "etc/mw_com_config.json",
    ],
    edition = "2024",
    env = {
        "MW_LOG_CONFIG_FILE": "score/mw/com/example/vehicle-dynamics-example/etc/logging.json",
    },
    features = ["link_std_cpp_lib"],
    visibility = ["//visibility:public"],
    deps = [
        ":vehicle-dynamics-example-lib",
        "//score/mw/com/example/vehicle-dynamics-example/com-api-gen",
        "//score/mw/com/rust:score_com",
        "@score_baselibs//score/log_rust/score_log",
        "@score_baselibs//score/log_rust/stdout_logger",
        "@score_communication_crate_index//:clap",
        "@score_communication_crate_index//:futures",
    ],
)

rust_binary(
    name = "body-control-module-example",
    srcs = ["body_control_main.rs"],
    data = [
        "etc/body_control_module_lola_config.json",
        "etc/logging.json",
    ],
    edition = "2024",
    env = {
        "MW_LOG_CONFIG_FILE": "score/mw/com/example/vehicle-dynamics-example/etc/logging.json",
    },
    features = ["link_std_cpp_lib"],
    visibility = ["//visibility:public"],
    deps = [
        ":vehicle-dynamics-example-lib",
        "//score/mw/com/example/vehicle-dynamics-example/com-api-gen",
        "//score/mw/com/rust:score_com",
        "@score_baselibs//score/log_rust/score_log",
        "@score_baselibs//score/log_rust/stdout_logger",
        "@score_communication_crate_index//:clap",
    ],
)

rust_test(
    name = "vehicle-dynamics-example-tokio-integration-test",
    srcs = ["tests_using_tokio_runtime.rs"],
    data = ["etc/mw_com_config.json"],
    edition = "2024",
    features = ["link_std_cpp_lib"],
    tags = ["manual"],
    deps = [
        "//score/mw/com/example/vehicle-dynamics-example/com-api-gen",
        "//score/mw/com/rust:score_com",
        "@score_communication_crate_index//:futures",
        "@score_communication_crate_index//:tokio",
    ],
)