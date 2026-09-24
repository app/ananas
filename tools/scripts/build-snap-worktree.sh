#!/usr/bin/env bash
# Build the Ananas .snap from the *current working tree* (including uncommitted
# changes), unlike build-snap.sh which exports the committed branch with
# `git archive`. Intended for testing local changes before they are committed.
#
# The official Snapcraft OCI image is used directly: no host Snapcraft, no LXD.
#
# Container engine: podman (preferred) or docker; override with CONTAINER=...
#
# Usage: build-snap-worktree.sh
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/../.." && pwd)"
IMAGE="${ANANAS_SNAP_IMAGE:-ghcr.io/canonical/snapcraft:8_core24}"

CONTAINER="${CONTAINER:-$(command -v podman || command -v docker || true)}"
if [[ -z "$CONTAINER" ]]; then
    echo "ERROR: podman or docker is required" >&2
    exit 1
fi

VERSION_STR="$(tr -d '\n' < "$REPO/VERSION")"
GIT_SHA="$(git -C "$REPO" rev-parse --short HEAD 2>/dev/null || echo nogit)"
ANANAS_VERSION="${VERSION_STR}+git$(date -u +%Y%m%d).${GIT_SHA}"

echo "===> Building the snap from the working tree (${IMAGE}), version ${ANANAS_VERSION}..."
"$CONTAINER" run --rm -e ANANAS_VERSION="$ANANAS_VERSION" \
    -v "$REPO":/project:z "$IMAGE" pack

mkdir -p "$REPO/dist"
cp -v "$REPO"/ananas_*.snap "$REPO/dist/"

echo "===> done: $REPO/dist"
