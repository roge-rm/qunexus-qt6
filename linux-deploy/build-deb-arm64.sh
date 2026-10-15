#!/usr/bin/env bash
# Cross-compiles build/qunexus-editor_<version>-<DEB_REVISION>_arm64.deb in a Debian trixie container.
# The compile goes in build/arm64. Set NO_BUILD=1 to reuse it, REBUILD_IMAGE=1 to rebuild the image.
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
image=qunexus-arm64-build

if [ "${REBUILD_IMAGE:-0}" = 1 ] || ! docker image inspect "$image" >/dev/null 2>&1; then
    docker build -t "$image" - <<'DOCKERFILE'
FROM debian:trixie
RUN dpkg --add-architecture arm64 \
 && apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    crossbuild-essential-arm64 dpkg-dev desktop-file-utils \
    qmake6 qt6-base-dev-tools qt6-declarative-dev-tools \
    qt6-base-dev:arm64 qt6-declarative-dev:arm64 libasound2-dev:arm64 \
 && rm -rf /var/lib/apt/lists/*
DOCKERFILE
fi

mkdir -p "$root/build/arm64"
# its own stash, or qmake finds the native build/.qmake.stash in the parent folder
touch "$root/build/arm64/.qmake.stash"
docker run --rm --user "$(id -u):$(id -g)" -e HOME=/tmp \
    -e NO_BUILD="${NO_BUILD:-0}" -e JOBS="${JOBS:-10}" \
    -e DEB_ARCH=arm64 -e BUILD_DIR=build/arm64 -e DEB_REVISION="${DEB_REVISION:-2}" \
    -e QMAKE=aarch64-linux-gnu-qmake6 -e STRIP=aarch64-linux-gnu-strip \
    -v "$root:/src" -w /src "$image" linux-deploy/build-deb.sh
