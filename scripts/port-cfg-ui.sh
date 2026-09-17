#!/usr/bin/env bash
# One-off: port the Qt3 .ui forms embedded in an Ananas scheme .cfg to the
# Qt4/Qt6 format (see port-cfg-ui.py).
#
# Runs in the Qt4 image, because `uic3` only exists there.
#
# Usage: port-cfg-ui.sh [cfg-path]
#   cfg-path defaults to ananas-legacy-qt4/applications/inventory/inventory.cfg
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
IMAGE="${ANANAS_IMAGE:-ananas-qt4-builder}"
CFG="${1:-ananas-legacy-qt4/applications/inventory/inventory.cfg}"

if [[ "$CFG" = /* ]]; then
    ABS="$CFG"
else
    ABS="$WORKSPACE_DIR/$CFG"
fi
REL="${ABS#"$WORKSPACE_DIR"/}"

if [[ ! -f "$ABS" ]]; then
    echo "cfg not found: $ABS" >&2
    exit 1
fi

echo "===> converting embedded forms in $REL (image: $IMAGE)"
podman run --rm \
    -v "$WORKSPACE_DIR":/workspace:z \
    "$IMAGE" \
    python3 /workspace/tools/scripts/port-cfg-ui.py "/workspace/$REL"
