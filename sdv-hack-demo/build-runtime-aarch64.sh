#!/usr/bin/env bash
# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# SPDX-License-Identifier: Apache-2.0

set -euo pipefail
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${repo_root}/patch"

if [[ ! -d sdv-runtime ]]; then
    git clone --recurse-submodules https://github.com/eclipse-autowrx/sdv-runtime.git
fi

cd sdv-runtime
apply_patch_once() {
    local patch="$1"
    if git apply --reverse --check "${patch}" 2>/dev/null; then
        printf 'Already applied: %s\n' "${patch}"
    else
        git apply --check "${patch}"
        git apply "${patch}"
    fi
}
apply_patch_once ../sdv-runtime-mw-com.patch
cp "${repo_root}/sdv-hack-demo/sdv-runtime/sdk_extensions.vspec" overlays/sdk_extensions.vspec
apply_patch_once "${repo_root}/sdv-hack-demo/sdv-runtime/runtime-model.patch"

docker buildx build \
    --platform linux/arm64 \
    --load \
    -t sdv-runtime:mw-com \
    -f Dockerfile .

docker run --rm --platform linux/arm64 -i --entrypoint python3 \
    -e "SDK_SIGNAL_TYPES=$(python3 -c 'import json, sys; print(json.dumps({m["vss_path"]: m["datatype"] for m in json.load(open(sys.argv[1]))["mappings"]}))' \
        "${repo_root}/sdv-hack-demo/config/gateway_provider_config.json")" \
    -e PYTHONPATH=/home/dev/python-packages sdv-runtime:mw-com - \
    < "${repo_root}/sdv-hack-demo/sdv-runtime/verify_sdk_model.py"

# 2. Save and load (only needed to move the image to another machine)
docker save -o sdv-runtime-mw-com.tar sdv-runtime:mw-com
docker load -i sdv-runtime-mw-com.tar
