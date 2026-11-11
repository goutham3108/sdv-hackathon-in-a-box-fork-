#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
address="${VDB_ADDRESS:-127.0.0.1:55555}"
provider_config="${root}/mw_com_provider/generated/mw_com_provider_config.json"
score_config="${root}/mw_com_provider/generated/bcm_config_lola_config.json"
service_manifest="${root}/etc/body_control_module_lola_config.json"
run_mode="${BODY_CONTROL_RUN_MODE:-both}"
broker_pid=""
app_pid=""

for required in databroker-mw-com databroker-cli body-control-module-example; do
    if [[ ! -x "${root}/${required}" ]]; then
        printf 'ERROR: Required BCM package executable is missing: %s\n' "${root}/${required}" >&2
        exit 1
    fi
done
for required in "${provider_config}" "${score_config}" "${service_manifest}"; do
    if [[ ! -f "${required}" ]]; then
        printf 'ERROR: Required BCM package config is missing: %s\n' "${required}" >&2
        exit 1
    fi
done

broker_port="${address##*:}"
if ss -Hltnp "sport = :${broker_port}" | rg -q .; then
    printf 'ERROR: TCP port %s is already in use; stop the other Databroker profile before starting BCM.\n' \
        "${broker_port}" >&2
    exit 1
fi

cleanup() {
    for pid in "${app_pid}" "${broker_pid}"; do
        if [[ -n "${pid}" ]] && kill -0 "${pid}" 2>/dev/null; then
            kill "${pid}" 2>/dev/null || true
        fi
    done
    for pid in "${app_pid}" "${broker_pid}"; do
        if [[ -n "${pid}" ]]; then
            wait "${pid}" 2>/dev/null || true
        fi
    done
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

cd "${root}"
"${root}/databroker-mw-com" \
    --address "${address}" \
    --provider-config "${provider_config}" \
    --score-config "${score_config}" &
broker_pid=$!

broker_host="${address%:*}"
broker_ready=false
for _ in $(seq 1 120); do
    if ! kill -0 "${broker_pid}" 2>/dev/null; then
        wait "${broker_pid}"
        printf 'ERROR: Databroker exited during startup.\n' >&2
        exit 1
    fi
    if timeout 1 bash -c ':</dev/tcp/$1/$2' _ "${broker_host}" "${broker_port}" 2>/dev/null; then
        broker_ready=true
        break
    fi
    sleep 0.25
done
if [[ "${broker_ready}" != true ]]; then
    printf 'ERROR: Databroker did not listen at %s.\n' "${address}" >&2
    exit 1
fi

"${root}/body-control-module-example" \
    --run-mode "${run_mode}" \
    --service-instance-manifest "${service_manifest}" &
app_pid=$!
wait "${app_pid}"
