#!/usr/bin/env bash
# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# SPDX-License-Identifier: Apache-2.0

set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${root}/network.env"
export SIGNAL_ROUTE_CONFIG="${root}/signal_routes.json"

# Refuse to start if another remote app instance is already running; two
# concurrent instances would both receive and echo the same UDP frame.
exec 9>"${root}/remote.lock"
if ! flock -n 9; then
    echo "Another remote app is already running (lock held on ${root}/remote.lock). Stop it before starting a new one." >&2
    exit 1
fi

export SDV_BIND_IP=0.0.0.0
export SDV_BRIDGE_IP="${SDV_VEHICLE_IP}"
export SDV_KVS_DIR="${root}/state"

pkill -TERM -x remote_app 2>/dev/null || true
for attempt in $(seq 1 20); do
	if ! pgrep -x remote_app >/dev/null; then
		break
	fi
	sleep 0.25
done
pkill -KILL -x remote_app 2>/dev/null || true

if ss -Hlun | grep -Eq ':35001([[:space:]]|$)'; then
	echo "UDP port 35001 is still in use; stop the owning process and retry." >&2
	exit 1
fi

vsomeip_lib_dir="$(find "${root}/remote_app.runfiles" -name libvsomeip3.so.3 -printf '%h\n' -quit)"
test -n "${vsomeip_lib_dir}"
export LD_LIBRARY_PATH="${vsomeip_lib_dir}${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"

exec "${root}/remote_app"
