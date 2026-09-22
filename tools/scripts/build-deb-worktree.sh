#!/usr/bin/env bash
# Build the ananas .deb packages from the *current working tree* (including
# uncommitted changes), unlike build-deb.sh which exports the committed branch
# with `git archive`. Intended for testing local changes before they are
# committed.
#
# Container engine: podman (preferred) or docker; override with CONTAINER=...
#
# Usage: build-deb-worktree.sh
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/../.." && pwd)"
IMAGE="${ANANAS_IMAGE:-ananas-qt6-builder}"

CONTAINER="${CONTAINER:-$(command -v podman || command -v docker || true)}"
if [[ -z "$CONTAINER" ]]; then
    echo "ERROR: podman or docker is required" >&2
    exit 1
fi

"$CONTAINER" run --rm \
    -v "$REPO":/repo:z \
    "$IMAGE" \
    bash -c '
        set -e
        export CCACHE_DIR=/repo/tmp/ccache
        mkdir -p "$CCACHE_DIR"

        rm -rf /tmp/ananas && mkdir -p /tmp/ananas
        tar -C /repo \
            --exclude=./.git --exclude=./bin --exclude=./lib --exclude=./dist \
            --exclude=./cmake-build --exclude=./tmp \
            --exclude="*.o" --exclude=.moc --exclude=.obj --exclude=.ui \
            -cf - . | tar -xf - -C /tmp/ananas
        find /tmp/ananas/src -name Makefile -delete

        cd /tmp/ananas
        dpkg-buildpackage -b -uc -us -d

        mkdir -p /repo/dist
        cp -v /tmp/ananas_*.deb /tmp/libqdataschema_*.deb /repo/dist/
        echo "===> done: /repo/dist"
    '
