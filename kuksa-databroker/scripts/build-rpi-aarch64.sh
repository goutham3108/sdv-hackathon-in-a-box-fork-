#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
kuksa_root="$(cd "${script_dir}/.." && pwd)"
workspace_root="$(cd "${kuksa_root}/.." && pwd)"
communication_root="${COMMUNICATION_ROOT:-${workspace_root}/communication}"
base_libs_root="${workspace_root}/baselibs"
target="aarch64-unknown-linux-gnu"
toolchain_root="${AARCH64_TOOLCHAIN_ROOT:-${HOME}/aarch64_toolchain}"

fail() {
    printf 'ERROR: %s\n' "$*" >&2
    exit 1
}

[[ "$(uname -m)" == "x86_64" ]] || fail "Run this script on an x86_64 Linux build host."
command -v bazel >/dev/null || fail "Bazel is required."
command -v rustup >/dev/null || fail "Install rustup and ensure ~/.cargo/bin is on PATH."
command -v file >/dev/null || fail "The 'file' command is required for architecture verification."

export PATH="${HOME}/.cargo/bin:${PATH}"
rust_version="$(rustup run stable rustc --version | awk '{print $2}')"
rust_minor="$(printf '%s' "$rust_version" | cut -d. -f2)"
[[ "$rust_minor" =~ ^[0-9]+$ ]] || fail "Could not parse Rust version: ${rust_version}"
(( rust_minor >= 96 )) || fail "Rust 1.96+ is required by the locked vergen dependencies; found ${rust_version}. Run: rustup update stable"
rustup target add --toolchain stable "$target"

if [[ ! -d "$communication_root" ]]; then
    if [[ -d "${HOME}/communication" ]]; then
        communication_root="${HOME}/communication"
    else
        fail "Communication checkout not found. Set COMMUNICATION_ROOT to it."
    fi
fi
if [[ ! -e "${workspace_root}/communication" ]]; then
    ln -s "$communication_root" "${workspace_root}/communication"
    printf 'Created workspace link: %s -> %s\n' "${workspace_root}/communication" "$communication_root"
fi
[[ -f "${base_libs_root}/score/log_rust/score_log/Cargo.toml" ]] || fail "Baselibs checkout missing at ${base_libs_root}."

cross_gxx="${AARCH64_CXX:-${toolchain_root}/toolchain/bin/aarch64-none-linux-gnu-g++}"
cross_gcc="${AARCH64_CC:-${toolchain_root}/toolchain/bin/aarch64-none-linux-gnu-gcc}"
cross_ar="${AARCH64_AR:-${toolchain_root}/toolchain/bin/aarch64-none-linux-gnu-ar}"
[[ -x "$cross_gxx" ]] || fail "AArch64 C++ linker not executable: ${cross_gxx}"
[[ -x "$cross_gcc" ]] || fail "AArch64 C compiler not executable: ${cross_gcc}"
[[ -x "$cross_ar" ]] || fail "AArch64 archiver not executable: ${cross_ar}"

printf '\n[1/6] Building Communication examples for AArch64...\n'
(
    cd "$communication_root"
    bazel build --config=linux_aarch64_score_gcc_12_2_0_posix \
    //score/mw/com/example/vehicle-dynamics-example:vehicle-dynamics-example \
    //score/mw/com/example/vehicle-dynamics-example:body-control-module-example
)

score_com_bazel_bin="$(cd "$communication_root" && bazel info bazel-bin)"
score_com_example_dir="${score_com_bazel_bin}/score/mw/com/example/vehicle-dynamics-example"
score_com_link_params="${score_com_example_dir}/body-control-module-example-0.params"
body_control_binary="${score_com_example_dir}/body-control-module-example"
body_control_manifest="${communication_root}/score/mw/com/example/vehicle-dynamics-example/etc/body_control_module_lola_config.json"
[[ -f "$score_com_link_params" ]] || fail "S-CORE link params not found: ${score_com_link_params}"
[[ -x "$body_control_binary" ]] || fail "BodyControl application was not built: ${body_control_binary}"
[[ -f "$body_control_manifest" ]] || fail "BodyControl LoLa manifest not found: ${body_control_manifest}"

export SCORE_COM_BAZEL_BIN="$score_com_bazel_bin"
export SCORE_COM_LINK_PARAMS="$score_com_link_params"
export SCORE_COM_VEHICLE_GEN_LIB_DIR="${score_com_bazel_bin}/score/mw/com/example/vehicle-dynamics-example/com-api-gen"
export CARGO_TARGET_AARCH64_UNKNOWN_LINUX_GNU_LINKER="$cross_gxx"
export CC_aarch64_unknown_linux_gnu="$cross_gcc"
export AR_aarch64_unknown_linux_gnu="$cross_ar"

cd "$kuksa_root"
printf '\n[2/6] Building Databroker, protobuf crate, and CLI...\n'
cargo +stable build --release --target "$target" \
    -p databroker -p databroker-proto -p databroker-cli

printf '\n[3/6] Building mw_com_provider library with score-lola...\n'
cargo +stable build --release --target "$target" \
    -p mw_com_provider --lib --features score-lola

printf '\n[4/6] Building mw_com_provider executable...\n'
cargo +stable build --release --target "$target" \
    -p mw_com_provider --bin mw_com_provider

printf '\n[5/6] Building databroker-mw-com...\n'
SCORE_COM_LINK_OWNER=databroker-mw-com \
    cargo +stable build --release --target "$target" \
        -p databroker-mw-com

printf '\n[6/6] Verifying ARM64 executable outputs...\n'
for binary in databroker databroker-cli mw_com_provider databroker-mw-com; do
    path="${kuksa_root}/target/${target}/release/${binary}"
    [[ -x "$path" ]] || fail "Expected executable was not built: ${path}"
    description="$(file -b "$path")"
    printf '%-28s %s\n' "$binary" "$description"
    [[ "$description" == *"ARM aarch64"* ]] || fail "Not an ARM64 executable: ${path}"
done
body_control_description="$(file -b "$body_control_binary")"
printf '%-28s %s\n' "body-control-module-example" "$body_control_description"
[[ "$body_control_description" == *"ARM aarch64"* ]] || fail "Not an ARM64 executable: ${body_control_binary}"

printf '\nAll requested Raspberry Pi targets built successfully.\n'
printf 'Output directory: %s/target/%s/release\n' "$kuksa_root" "$target"
printf 'BodyControl application: %s\n' "$body_control_binary"
printf 'BodyControl LoLa manifest: %s\n' "$body_control_manifest"
