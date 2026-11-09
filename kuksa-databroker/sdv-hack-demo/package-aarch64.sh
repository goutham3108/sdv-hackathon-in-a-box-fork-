#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
gateway_root="${repo_root}/inc_someip_gateway"
gateway_bazel_bin="${gateway_root}/bazel-out/aarch64-fastbuild/bin"
demo_bazel_bin="${repo_root}/bazel-out/aarch64-fastbuild/bin"
dist_dir="${DIST_DIR:-${repo_root}/sdv-hack-demo/dist}"
kuksa_release_dir="${repo_root}/kuksa-databroker/target/aarch64-unknown-linux-gnu/release"
communication_root="${COMMUNICATION_ROOT:-${repo_root}/communication}"
communication_bazel_bin="$(cd "${communication_root}" && bazel info bazel-bin)"
body_control_bazel_dir="${communication_bazel_bin}/score/mw/com/example/vehicle-dynamics-example"
staging_dir="$(mktemp -d)"
trap 'rm -rf "${staging_dir}"' EXIT

vehicle_dir="${staging_dir}/vehicle"
remote_dir="${staging_dir}/remote"
body_control_dir="${staging_dir}/body-control"
mkdir -p "${vehicle_dir}/run" "${remote_dir}/run"
mkdir -p "${vehicle_dir}/mw_com_provider/generated"
mkdir -p "${body_control_dir}/run" "${body_control_dir}/etc"
mkdir -p "${body_control_dir}/mw_com_provider/generated"

copy_target() {
    local target_dir="$1"
    local binary="$2"
    local runfiles="$3"

    if [[ ! -f "${binary}" ]]; then
        printf 'ERROR: Required build artifact not found: %s\nRun sdv-hack-demo/build-aarch64.sh first.\n' \
            "${binary}" >&2
        return 1
    fi
    if [[ ! -d "${runfiles}" ]]; then
        printf 'ERROR: Required Bazel runfiles directory not found: %s\nRun sdv-hack-demo/build-aarch64.sh first.\n' \
            "${runfiles}" >&2
        return 1
    fi

    cp -a "${binary}" "${target_dir}/"
    cp -aL "${runfiles}" "${target_dir}/"
}

copy_target "${vehicle_dir}" \
    "${gateway_bazel_bin}/score/someipd/someipd" \
    "${gateway_bazel_bin}/score/someipd/someipd.runfiles"
copy_target "${vehicle_dir}" \
    "${gateway_bazel_bin}/score/gatewayd/gatewayd" \
    "${gateway_bazel_bin}/score/gatewayd/gatewayd.runfiles"
copy_target "${vehicle_dir}" \
    "${demo_bazel_bin}/sdv-hack-demo/bridge/udp_bridge" \
    "${demo_bazel_bin}/sdv-hack-demo/bridge/udp_bridge.runfiles"
copy_target "${vehicle_dir}" \
    "${demo_bazel_bin}/sdv-hack-demo/vehicle_app/score_app" \
    "${demo_bazel_bin}/sdv-hack-demo/vehicle_app/score_app.runfiles"
copy_target "${vehicle_dir}" \
    "${demo_bazel_bin}/sdv-hack-demo/vehicle_app/body_signal_test_app" \
    "${demo_bazel_bin}/sdv-hack-demo/vehicle_app/body_signal_test_app.runfiles"
cp -a "${kuksa_release_dir}/databroker-mw-com" "${vehicle_dir}/"
cp -a "${kuksa_release_dir}/databroker-cli" "${vehicle_dir}/"
cp -a "${repo_root}/kuksa-databroker/mw_com_provider/generated/mw_com_provider_config.json" \
    "${vehicle_dir}/mw_com_provider/generated/"
cp -a "${repo_root}/sdv-hack-demo/config/gateway_provider_config.json" \
    "${vehicle_dir}/mw_com_provider/generated/"
cp -a "${repo_root}/kuksa-databroker/mw_com_provider/generated/bcm_config_lola_config.json" \
    "${vehicle_dir}/mw_com_provider/generated/"
copy_target "${remote_dir}" \
    "${demo_bazel_bin}/sdv-hack-demo/remote_app/remote_app" \
    "${demo_bazel_bin}/sdv-hack-demo/remote_app/remote_app.runfiles"
copy_target "${body_control_dir}" \
    "${body_control_bazel_dir}/body-control-module-example" \
    "${body_control_bazel_dir}/body-control-module-example.runfiles"

body_control_description="$(file -b "${body_control_dir}/body-control-module-example")"
if [[ "${body_control_description}" != *"ARM aarch64"* ]]; then
    printf 'ERROR: BodyControl executable is not ARM64: %s\n' "${body_control_description}" >&2
    exit 1
fi

cp -a "${kuksa_release_dir}/databroker-mw-com" "${body_control_dir}/"
cp -a "${kuksa_release_dir}/databroker-cli" "${body_control_dir}/"
cp -a "${repo_root}/kuksa-databroker/mw_com_provider/generated/mw_com_provider_config.json" \
    "${body_control_dir}/mw_com_provider/generated/"
cp -a "${repo_root}/kuksa-databroker/mw_com_provider/generated/bcm_config_lola_config.json" \
    "${body_control_dir}/mw_com_provider/generated/"
cp -a "${communication_root}/score/mw/com/example/vehicle-dynamics-example/etc/body_control_module_lola_config.json" \
    "${body_control_dir}/etc/"
cp -a "${repo_root}/sdv-hack-demo/deploy/start-body-control.sh" "${body_control_dir}/run/"

cp -a "${demo_bazel_bin}/sdv-hack-demo/mw_someip_config.bin" "${vehicle_dir}/"
cp -a "${gateway_root}/score/gatewayd/etc/mw_com_config.json" "${vehicle_dir}/"
cp -a "${gateway_root}/tests/integration/vsomeip-gateway-services.json" "${vehicle_dir}/"
# gatewayd loads score_com_serializer.so; the demo plugin forwards to the gateway's own
# serializer, shipped next to it as score_com_serializer_base.so.
cp -a "${demo_bazel_bin}/sdv-hack-demo/serializer/score_com_serializer.so" "${vehicle_dir}/"
cp -a "${demo_bazel_bin}/sdv-hack-demo/serializer/score_com_serializer_base.so" "${vehicle_dir}/"
cp -a "${repo_root}/sdv-hack-demo/deploy/start-vehicle.sh" "${vehicle_dir}/run/"
cp -a "${repo_root}/sdv-hack-demo/deploy/start-remote.sh" "${remote_dir}/run/"
cp -a "${repo_root}/sdv-hack-demo/deploy/network.env" "${vehicle_dir}/"
cp -a "${repo_root}/sdv-hack-demo/deploy/network.env" "${remote_dir}/"
cp -a "${repo_root}/sdv-hack-demo/signal_routes.json" "${vehicle_dir}/"
cp -a "${repo_root}/sdv-hack-demo/signal_routes.json" "${remote_dir}/"
chmod +x "${vehicle_dir}/run/start-vehicle.sh" "${remote_dir}/run/start-remote.sh"
chmod +x "${body_control_dir}/run/start-body-control.sh"

mkdir -p "${dist_dir}"
tar -chzf "${dist_dir}/vehicle-aarch64.tar.gz" -C "${vehicle_dir}" .
tar -chzf "${dist_dir}/remote-aarch64.tar.gz" -C "${remote_dir}" .
tar -chzf "${dist_dir}/body-control-aarch64.tar.gz" -C "${body_control_dir}" .

printf 'Created:\n  %s\n  %s\n' \
    "${dist_dir}/vehicle-aarch64.tar.gz" \
    "${dist_dir}/remote-aarch64.tar.gz" \
    "${dist_dir}/body-control-aarch64.tar.gz"