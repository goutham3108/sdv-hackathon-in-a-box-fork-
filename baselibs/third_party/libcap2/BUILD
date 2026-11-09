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

alias(
    name = "libcap2",
    actual = select({
        "@platforms//cpu:aarch64": "@libcap2-dev-deb-aarch64//:libcap2",
        "//conditions:default": "@libcap2-dev-deb//:libcap2",
    }),
    visibility = ["//:__subpackages__"],
)
