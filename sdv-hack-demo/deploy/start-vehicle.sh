#!/usr/bin/env bash
# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# SPDX-License-Identifier: Apache-2.0

set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${root}/network.env"

# Refuse to start if another vehicle stack is already running; running two
# stacks at once causes each Vehicle.speed publish to be duplicated and
# delivered with a second, stale process's state (seen as spurious 0 values).
exec 9>"${root}/vehicle.lock"
if ! flock -n 9; then
    echo "Another vehicle stack is already running (lock held on ${root}/vehicle.lock). Stop it before starting a new one." >&2
    exit 1
fi

export SDV_BIND_IP=0.0.0.0
export SIGNAL_ROUTE_CONFIG="${root}/signal_routes.json"
vehicle_app_mode="${VEHICLE_APP_MODE:-combined}"
if [[ "${vehicle_app_mode}" != signals && ! -x "${root}/databroker-mw-com" ]]; then
    echo "Required broker executable missing or not executable: ${root}/databroker-mw-com. Redeploy the complete vehicle archive." >&2
    exit 1
fi
export HIGH_BEAM_COMMAND_MODE="${HIGH_BEAM_COMMAND_MODE:-kuksa}"
case "$HIGH_BEAM_COMMAND_MODE" in
    kuksa) export KUKSA_HIGH_BEAM_ACTUATION=1 ;;
    direct) export KUKSA_HIGH_BEAM_ACTUATION=0 ;;
    *) echo "HIGH_BEAM_COMMAND_MODE must be kuksa or direct" >&2; exit 2 ;;
esac
export VDB_ADDRESS="${VDB_ADDRESS:-127.0.0.1:55555}"

ensure_broker_port_free() {
    local listeners
    listeners="$(ss -Hltnp 'sport = :55555')"
    if [[ -n "$listeners" ]]; then
        echo "TCP port 55555 is already occupied; the demo broker cannot start." >&2
        echo "$listeners" >&2
        echo "Identify and stop that listener before restarting this vehicle stack." >&2
        return 1
    fi
}

cp "${root}/vsomeip-gateway-services.json" "${root}/vsomeip-vehicle.json"
sed -i "s/\"unicast\": \"127.0.0.1\"/\"unicast\": \"${SDV_VEHICLE_IP}\"/" \
    "${root}/vsomeip-vehicle.json"

vsomeip_lib_dir="$(find "${root}/someipd.runfiles" -name libvsomeip3.so.3 -printf '%h\n' -quit)"
test -n "${vsomeip_lib_dir}"
export LD_LIBRARY_PATH="${root}:${vsomeip_lib_dir}${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"

stale_pattern='someipd|gatewayd|(^|/)udp_bridge([[:space:]]|$)|(^|/)score_app([[:space:]]|$)|body_signal_test_app|databroker-mw-com|start_kuksa'
# Stop a previous launcher's KUKSA retry loop first, otherwise it respawns the broker and steals the port.
pkill -KILL -f 'start_kuksa' 2>/dev/null || true
pkill -TERM -f "${stale_pattern}" 2>/dev/null || true
for attempt in $(seq 1 20); do
    if ! pgrep -f "${stale_pattern}" >/dev/null; then
        break
    fi
    sleep 0.25
done
pkill -KILL -f "${stale_pattern}" 2>/dev/null || true
if [[ "${vehicle_app_mode}" != signals ]]; then
    ensure_broker_port_free
fi
rm -f /tmp/vsomeip*.lck /tmp/vsomeip-[0-9]*

# mw::com (LoLa) shared-memory ring buffers and the gateway's counterpart SHM
# files persist across process restarts. Without clearing them, a freshly
# started subscriber immediately replays every stale sample left over from
# earlier runs (seen as a burst of old/garbled values on (re)subscription).
rm -f /dev/shm/lola-*-0000000000017152-* /dev/shm/lola-*-0000000000006433-* \
      /dev/shm/*vehicle_high_beam* /dev/shm/*VehicleDynamicsService* 2>/dev/null || true

if ss -Hlun | grep -Eq ':35000([[:space:]]|$)'; then
    echo "UDP port 35000 is still in use; stop the owning process and retry." >&2
    exit 1
fi

VSOMEIP_CONFIGURATION="${root}/vsomeip-vehicle.json" \
    setsid nohup "${root}/someipd" --configuration "${root}/mw_someip_config.bin" >"${root}/someipd.log" 2>&1 < /dev/null 9>&- &
disown

setsid nohup "${root}/gatewayd" \
    --configuration "${root}/mw_someip_config.bin" \
    --service_instance_manifest "${root}/mw_com_config.json" >"${root}/gatewayd.log" 2>&1 < /dev/null 9>&- &
disown

# gatewayd's connection to someipd's "someipd_gatewayd_ipc" IPC socket has been
# observed (especially under constrained/virtualized CPU scheduling) to connect
# once, then get closed by someipd (StopReason::kClosedByPeer) and never
# reconnect; a fresh gatewayd alone cannot recover, but restarting BOTH someipd
# and gatewayd together does. Detect the stuck case (repeated retry log lines,
# no ESTABLISHED abstract socket) and self-heal before proceeding.
for ipc_attempt in $(seq 1 5); do
    sleep 3
    if grep -q "@someipd_gatewayd_ipc" /proc/net/unix 2>/dev/null && \
       awk '$0 ~ /@someipd_gatewayd_ipc/ { print $6 }' /proc/net/unix | grep -q '^03$'; then
        break
    fi
    echo "gatewayd/someipd IPC not yet connected (attempt ${ipc_attempt}/5); restarting both." >&2
    # someipd rewrites its cmdline to plain "someipd", so match names, not paths.
    pkill -KILL -x someipd 2>/dev/null || true
    pkill -KILL -f 'gatewayd' 2>/dev/null || true
    for wait_attempt in $(seq 1 20); do
        if ! pgrep -x someipd >/dev/null && ! pgrep -f 'gatewayd' >/dev/null; then
            break
        fi
        sleep 0.25
    done
    rm -f /tmp/vsomeip*.lck /tmp/vsomeip-[0-9]*
    VSOMEIP_CONFIGURATION="${root}/vsomeip-vehicle.json" \
        setsid nohup "${root}/someipd" --configuration "${root}/mw_someip_config.bin" >>"${root}/someipd.log" 2>&1 < /dev/null 9>&- &
    disown
    sleep 2
    setsid nohup "${root}/gatewayd" \
        --configuration "${root}/mw_someip_config.bin" \
        --service_instance_manifest "${root}/mw_com_config.json" >>"${root}/gatewayd.log" 2>&1 < /dev/null 9>&- &
    disown
done

VSOMEIP_CONFIGURATION="${root}/vsomeip-vehicle.json" \
VEHICLE_DOMAIN_CONFIG="${root}/vsomeip-vehicle.json" \
    setsid nohup "${root}/udp_bridge" >"${root}/bridge.log" 2>&1 < /dev/null 9>&- &
disown

# Give gatewayd time to complete its someipd handshake and create the
# remote-instance shared memory before the selected vehicle app starts.
sleep 10

# A crashed gateway daemon must not leave a half-running stack that only fails later.
require_gateway_alive() {
    local name
    for name in someipd gatewayd; do
        if ! pgrep -x "$name" >/dev/null; then
            echo "${name} is not running; see ${root}/${name}.log" >&2
            tail -n 20 "${root}/${name}.log" >&2 || true
            pkill -KILL -x someipd 2>/dev/null || true
            pkill -KILL -x gatewayd 2>/dev/null || true
            pkill -KILL -f udp_bridge 2>/dev/null || true
            return 1
        fi
    done
}
require_gateway_alive || exit 1

cd "${root}"
start_kuksa() {
    # gatewayd creates its remote instances asynchronously; KUKSA's proxy creation fails if the
    # shared memory is not there yet. Wait for it, then let the retry loop cover any remaining race.
    for attempt in $(seq 1 60); do
        if [[ -e /dev/shm/lola-data-0000000000006433-00002 && \
              -e /dev/shm/lola-data-0000000000017152-04097 ]]; then
            break
        fi
        sleep 0.5
    done
    for attempt in $(seq 1 5); do
        "${root}/databroker-mw-com" \
            --address 0.0.0.0:55555 \
            --provider-config "${root}/mw_com_provider/generated/gateway_provider_config.json" \
            --score-config "${root}/mw_com_config.json" && return 0
        echo "KUKSA exited (attempt ${attempt}/5); retrying." >&2
        sleep 2
    done
    return 1
}

case "${vehicle_app_mode}" in
    signals)
        exec "${root}/body_signal_test_app" \
            --configuration "${root}/mw_com_config.json" \
            --routes "${root}/signal_routes.json" "$@"
        ;;
    combined)
        export -f start_kuksa
        export root
        setsid nohup bash -c start_kuksa >"${root}/kuksa.log" 2>&1 < /dev/null 9>&- &
        kuksa_launcher_pid=$!
        disown
        if [[ "$HIGH_BEAM_COMMAND_MODE" == kuksa ]]; then
            broker_host="${VDB_ADDRESS%:*}"
            broker_port="${VDB_ADDRESS##*:}"
            broker_ready=false
            # Budget: up to 30 s for gateway shared memory, then KUKSA start-up and retries.
            for attempt in $(seq 1 240); do
                if ! kill -0 "$kuksa_launcher_pid" 2>/dev/null; then
                    echo "Demo broker exited during startup; see ${root}/kuksa.log" >&2
                    exit 1
                fi
                if timeout 1 bash -c ':</dev/tcp/$1/$2' _ "$broker_host" "$broker_port" 2>/dev/null; then
                    broker_ready=true
                    break
                fi
                sleep 0.5
            done
            if [[ "$broker_ready" != true ]]; then
                echo "KUKSA is not listening at $VDB_ADDRESS; see ${root}/kuksa.log" >&2
                exit 1
            fi
        fi
        exec "${root}/score_app" --configuration "${root}/mw_com_config.json"
        ;;
    *)
        echo "Unknown VEHICLE_APP_MODE '${vehicle_app_mode}'; choose 'combined' or 'signals'." >&2
        exit 2
        ;;
esac
