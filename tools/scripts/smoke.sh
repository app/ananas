#!/usr/bin/env bash
# Phase 4 smoke harness: build ananas-legacy-qt4 in the Qt6 container and run
# the QtTest suite headlessly under Xvfb.
#
# The tree is cleaned of previous build artifacts first, because the generated
# Makefiles/objects from the Qt4/Qt5 images are not reusable.
#
# Usage: smoke-qt6.sh [path-to-ananas-legacy-qt4]
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="${1:-$SCRIPT_DIR/../../ananas-legacy-qt4}"
IMAGE="${ANANAS_IMAGE:-ananas-qt6-builder}"

if [[ ! -d "$REPO" ]]; then
    echo "Repository not found: $REPO" >&2
    exit 1
fi

REPO="$(cd "$REPO" && pwd)"
WORKSPACE="$(dirname "$REPO")"
REPO_NAME="$(basename "$REPO")"

podman run --rm \
    -v "$WORKSPACE":/workspace:z \
    -w "/workspace/$REPO_NAME" \
    "$IMAGE" \
    bash -c '
        set -e

        export CCACHE_DIR=/workspace/tmp/ccache
        mkdir -p "$CCACHE_DIR"
        REPO_DIR="$PWD"

        echo "===> Cleaning previous build artifacts..."
        find src -name Makefile -delete
        rm -rf lib bin
        find src -type d \( -name .obj -o -name .moc -o -name .ui \) -prune -exec rm -rf {} +

        echo "===> Building the project..."
        if ! make >/tmp/build.log 2>&1; then
            echo "BUILD FAILED"; tail -40 /tmp/build.log; exit 1
        fi
        echo "     build ok"

        echo "===> Building ananas-test..."
        cd src/test
        qmake test.pro -o Makefile >/dev/null 2>&1
        if ! make >/tmp/test-build.log 2>&1; then
            echo "TEST BUILD FAILED"; grep -E "error:|fatal error|undefined reference" /tmp/test-build.log | head -20; exit 1
        fi

        echo "===> Running ananas-test (Xvfb)..."
        export LD_LIBRARY_PATH="$REPO_DIR/lib:$REPO_DIR/lib/designer"
        xvfb-run -a ./ananas-test

        echo "===> ccache stats:"
        ccache -s
    '
