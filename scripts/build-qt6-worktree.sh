#!/usr/bin/env bash
# Build the ananas .deb from the *current working tree* (including uncommitted
# changes), unlike build-qt6.sh which exports the committed branch with
# `git archive`. Intended for testing local changes before they are committed.
#
# Usage: build-qt6-worktree.sh
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
REPO_NAME="ananas-legacy-qt4"
IMAGE="${ANANAS_IMAGE:-ananas-qt6-builder}"

if [[ ! -d "$WORKSPACE_DIR/$REPO_NAME" ]]; then
    echo "Repository not found: $WORKSPACE_DIR/$REPO_NAME" >&2
    exit 1
fi

podman run --rm \
    -v "$WORKSPACE_DIR":/workspace:z \
    -e REPO_NAME="$REPO_NAME" \
    "$IMAGE" \
    bash -c '
        set -e
        export CCACHE_DIR=/workspace/tmp/ccache
        mkdir -p "$CCACHE_DIR"

        rm -rf /tmp/ananas && mkdir -p /tmp/ananas
        tar -C /workspace/"$REPO_NAME" \
            --exclude=./.git --exclude=./bin --exclude=./lib --exclude=./tmp \
            --exclude="*.o" --exclude=.moc --exclude=.obj --exclude=.ui \
            -cf - . | tar -xf - -C /tmp/ananas
        find /tmp/ananas/src -name Makefile -delete

        cd /tmp/ananas
        echo "libqdataschema 1 libqdataschema (>= 1.0.0)" > debian/shlibs.local
        dpkg-buildpackage -b -uc -us -d

        mkdir -p /workspace/dist
        cp -v /tmp/ananas_*.deb /workspace/dist/
        echo "===> done: /workspace/dist"
    '
