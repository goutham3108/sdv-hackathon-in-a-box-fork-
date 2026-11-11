#!/usr/bin/env bash
# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# SPDX-License-Identifier: Apache-2.0

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
gateway_root="${repo_root}/inc_someip_gateway"

python3 "${repo_root}/sdv-hack-demo/configure_signals.py" --write

cd "${gateway_root}"
bazel build --config=aarch64-linux \
  //score/gatewayd \
  //score/someipd

cd "${gateway_root}"
bazel build --config=aarch64-linux --config=sdv-demo \
  --package_path="%workspace%:${repo_root}" \
  //sdv-hack-demo:gateway_config \
  //sdv-hack-demo/serializer:serializer_shim \
  //sdv-hack-demo/serializer:base_serializer \
  //sdv-hack-demo/vehicle_app:score_app \
  //sdv-hack-demo/vehicle_app:body_signal_test_app \
  //sdv-hack-demo/bridge:udp_bridge \
  //sdv-hack-demo/remote_app:remote_app

"${repo_root}/kuksa-databroker/scripts/build-rpi-aarch64.sh"
bash "${repo_root}/sdv-hack-demo/build-runtime-aarch64.sh"
#bash "${repo_root}/sdv-hack-demo/package-aarch64.sh"
