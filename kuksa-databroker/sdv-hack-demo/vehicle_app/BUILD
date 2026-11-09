# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_library(
    name = "signal_test_values",
    hdrs = ["signal_test_values.h"],
    deps = ["@nlohmann_json//:json"],
)

cc_test(
    name = "signal_test_values_test",
    srcs = ["signal_test_values_test.cpp"],
    data = ["//sdv-hack-demo:signal_routes.json"],
    deps = [
        ":signal_test_values",
    ],
)

cc_binary(
    name = "body_signal_test_app",
    srcs = ["body_signal_test_app.cpp"],
    data = ["//sdv-hack-demo:signal_routes.json"],
    visibility = ["//visibility:public"],
    deps = [
        ":signal_test_values",
        "@score_communication//score/mw/com",
        "@score_someip_gateway//score/serializer:pre_serialized_data",
    ],
)

cc_binary(
    name = "score_app",
    srcs = ["main.cc"],
    data = ["@score_someip_gateway//score/gatewayd:etc/mw_com_config.json"],
    visibility = ["//visibility:public"],
    deps = [
        "//:kuksa_val_client",
        "@grpc//:grpc++",
        "@score_lifecycle//score/launch_manager:lifecycle_cc",
        "@score_logging//score/mw/log:log",
        "@score_communication//score/mw/com",
        "@score_someip_gateway//score/serializer:pre_serialized_data",
    ],
)