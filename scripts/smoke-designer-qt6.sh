#!/usr/bin/env bash
# Phase 6 smoke harness: build the Qt6 form-designer wrapper and the
# ananas-designer application, then run two checks under Xvfb:
#   1. the wrapper open/save round-trip on a form from the inventory scheme;
#   2. the designer application starts (MainForm construction) via --help.
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
    -w "/workspace/$REPO_NAME/src/designer" \
    "$IMAGE" \
    bash -c '
        set -e

        export CCACHE_DIR=/workspace/tmp/ccache
        mkdir -p "$CCACHE_DIR"

        echo "===> Building the main tree (widget plugin, libs)..."
        make -C /workspace/'"$REPO_NAME"' -j"$(nproc)" >/tmp/main-build.log 2>&1 \
            || { echo "MAIN BUILD FAILED"; tail -40 /tmp/main-build.log; exit 1; }

        echo "===> Building the designer wrapper smoke target..."
        ( cd designer6 && qmake designer6.pro -o Makefile >/dev/null \
            && make -j"$(nproc)" >/tmp/designer6-build.log 2>&1 ) \
            || { echo "WRAPPER BUILD FAILED"; grep -E "error:|fatal error" /tmp/designer6-build.log | head; exit 1; }

        echo "===> Building ananas-designer..."
        qmake designer.pro -o Makefile >/dev/null
        make -j"$(nproc)" >/tmp/designer-build.log 2>&1 \
            || { echo "DESIGNER BUILD FAILED"; grep -E "error:|fatal error" /tmp/designer-build.log | head -20; exit 1; }

        export QT_PLUGIN_PATH=/workspace/'"$REPO_NAME"'/lib
        export LD_LIBRARY_PATH=/workspace/'"$REPO_NAME"'/lib:/workspace/'"$REPO_NAME"'/lib/designer:/workspace/'"$REPO_NAME"'/src/editor

        echo "===> Running the wrapper smoke (Xvfb)..."
        timeout 120 xvfb-run -a ../../bin/designer-smoke \
            /workspace/tmp/designer-fixtures/inventory-form-'"$FIXTURE_FORM"'.ui

        echo "===> Running ananas-designer --help (Xvfb)..."
        out=$(timeout 120 xvfb-run -a ../../bin/ananas-designer --help 2>&1 || true)
        if echo "$out" | grep -q "Usage: ananas-designer"; then
            echo "     designer start ok"
        else
            echo "DESIGNER START FAILED"; echo "$out" | tail -30; exit 1
        fi

        echo "===> ccache stats:"
        ccache -s | head -8
    '
