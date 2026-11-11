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

# Export the image for packaging and loading on the Pi.
docker save -o sdv-runtime-mw-com.tar sdv-runtime:mw-com
