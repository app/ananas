#!/usr/bin/env bash
# Build the Ananas .snap from the *current working tree* (including uncommitted
# changes), unlike build-snap.sh which exports the committed branch with
# `git archive`. Intended for testing local changes before they are committed.
#
# The working tree is staged into a clean temporary copy (no .git, no build
# outputs) before being mounted into the Snapcraft OCI image, so stale host
# artifacts (bin/, lib/, cmake-build/) can never leak into the build.
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

WORK="$(mktemp -d /tmp/ananas-snap.XXXXXX)"
trap 'rm -rf "$WORK"' EXIT

echo "===> 1. Staging the working tree..."
tar -C "$REPO" \
    --exclude=./.git --exclude=./bin --exclude=./lib --exclude=./dist \
    --exclude=./cmake-build --exclude=./tmp \
    --exclude=./parts --exclude=./stage --exclude=./prime --exclude=./.craft \
    --exclude="*.o" --exclude=.moc --exclude=.obj --exclude=.ui \
    --exclude="*.snap" \
    -cf - . | tar -xf - -C "$WORK"

VERSION_STR="$(tr -d '\n' < "$REPO/VERSION")"
GIT_SHA="$(git -C "$REPO" rev-parse --short HEAD 2>/dev/null || echo nogit)"
ANANAS_VERSION="${VERSION_STR}+git$(date -u +%Y%m%d).${GIT_SHA}"

echo "===> 2. Building the snap (${IMAGE}), version ${ANANAS_VERSION}..."
"$CONTAINER" run --rm -e ANANAS_VERSION="$ANANAS_VERSION" \
    -v "$WORK":/project:z "$IMAGE" pack

mkdir -p "$REPO/dist"
shopt -s nullglob
snaps=("$WORK"/ananas_*.snap)
if (( ${#snaps[@]} == 0 )); then
    echo "ERROR: no .snap was produced" >&2
    exit 1
fi
mv -f "${snaps[@]}" "$REPO/dist/"

echo "===> done: $REPO/dist"
