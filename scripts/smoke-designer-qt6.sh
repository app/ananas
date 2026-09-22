#!/usr/bin/env bash
# Phase 6 smoke harness: build the Qt6 form-designer wrapper and exercise the
# open/save round-trip on a form extracted from the inventory scheme.
#
# The fixture is extracted on the host (python3), then the wrapper is built and
# run inside the Qt6 image. The Ananas widget plugin must be available on the
# Designer plugin path; the main tree is built first for that.
#
# Usage: smoke-designer-qt6.sh [path-to-ananas-legacy-qt4]
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="${1:-$SCRIPT_DIR/../../ananas-legacy-qt4}"
IMAGE="${ANANAS_IMAGE:-ananas-qt6-builder}"
FIXTURE_FORM="${DESIGNER_FIXTURE_FORM:-406}"

if [[ ! -d "$REPO" ]]; then
    echo "Repository not found: $REPO" >&2
    exit 1
fi

REPO="$(cd "$REPO" && pwd)"
WORKSPACE="$(dirname "$REPO")"
REPO_NAME="$(basename "$REPO")"
FIXTURE_DIR="$WORKSPACE/tmp/designer-fixtures"
FIXTURE_UI="$FIXTURE_DIR/inventory-form-$FIXTURE_FORM.ui"

echo "===> Extracting fixture form $FIXTURE_FORM..."
mkdir -p "$FIXTURE_DIR"
python3 "$SCRIPT_DIR/extract-cfg-form.py" \
    "$REPO/applications/inventory/inventory.cfg" \
    "$FIXTURE_FORM" "$FIXTURE_UI"

podman run --rm \
    -v "$WORKSPACE":/workspace:z \
    -w "/workspace/$REPO_NAME/src/designer/designer6" \
    "$IMAGE" \
    bash -c '
        set -e

        export CCACHE_DIR=/workspace/tmp/ccache
        mkdir -p "$CCACHE_DIR"

        echo "===> Building the main tree (widget plugin)..."
        make -C /workspace/'"$REPO_NAME"' -j"$(nproc)" >/tmp/main-build.log 2>&1 \
            || { echo "MAIN BUILD FAILED"; tail -40 /tmp/main-build.log; exit 1; }

        echo "===> Building the designer wrapper..."
        qmake designer6.pro -o Makefile >/dev/null
        if ! make -j"$(nproc)" >/tmp/designer-build.log 2>&1; then
            echo "DESIGNER BUILD FAILED"
            grep -E "error:|fatal error" /tmp/designer-build.log | head -20
            exit 1
        fi

        echo "===> Running the designer smoke (Xvfb)..."
        export QT_PLUGIN_PATH=/workspace/'"$REPO_NAME"'/lib
        export LD_LIBRARY_PATH=/workspace/'"$REPO_NAME"'/lib:/workspace/'"$REPO_NAME"'/lib/designer
        timeout 120 xvfb-run -a ../../../bin/designer-smoke \
            /workspace/tmp/designer-fixtures/inventory-form-'"$FIXTURE_FORM"'.ui

        echo "===> ccache stats:"
        ccache -s | head -8
    '
