#!/usr/bin/env bash
# Build the Ananas .snap from the committed tree in the official Snapcraft OCI
# image (no host Snapcraft, no LXD/Multipass).
#
# Container engine: podman (preferred) or docker; override with CONTAINER=...
#
# Usage: build-snap.sh
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SCRIPT_DIR/../.." && pwd)"
BRANCH="${ANANAS_BRANCH:-master}"
IMAGE="${ANANAS_SNAP_IMAGE:-ghcr.io/canonical/snapcraft:8_core24}"

CONTAINER="${CONTAINER:-$(command -v podman || command -v docker || true)}"
if [[ -z "$CONTAINER" ]]; then
    echo "ERROR: podman or docker is required" >&2
    exit 1
fi

# git archive only exports tracked files: fail fast if a source file exists on
# disk but is ignored by .gitignore (it would silently be missing from the build).
MISSING="$(git -C "$REPO" ls-files --others --ignored --exclude-standard \
    | grep -E '\.(h|hpp|cpp|cc|cxx|ui|qrc)$' \
    | grep -vE '^(cmake-build|bin|lib|dist|tmp|parts|stage|prime)/' \
    | grep -vE '/(\.moc|\.obj|\.ui)/' \
    | grep -vE '(^|/)moc_predefs\.h$' \
    | grep -vE '(^|/)(moc_|qrc_)[^/]*\.cpp$' || true)"
if [[ -n "$MISSING" ]]; then
    echo "ERROR: ignored source files would be missing from the build:" >&2
    echo "$MISSING" >&2
    echo "Fix .gitignore or 'git add' them before packaging." >&2
    exit 1
fi

WORK="$(mktemp -d /tmp/ananas-snap.XXXXXX)"
trap 'rm -rf "$WORK"' EXIT

echo "===> 1. Exporting branch ${BRANCH}..."
git -C "$REPO" archive "${BRANCH}" | tar -x -C "$WORK"

VERSION_STR="$(tr -d '\n' < "$REPO/VERSION")"
GIT_SHA="$(git -C "$REPO" rev-parse --short "${BRANCH}" 2>/dev/null || echo nogit)"
ANANAS_VERSION="${VERSION_STR}+git$(date -u +%Y%m%d).${GIT_SHA}"

echo "===> 2. Building the snap (${IMAGE}), version ${ANANAS_VERSION}..."
"$CONTAINER" run --rm -e ANANAS_VERSION="$ANANAS_VERSION" \
    -v "$WORK":/project:z "$IMAGE" pack

mkdir -p "$REPO/dist"
cp -v "$WORK"/*.snap "$REPO/dist/"

echo "===> done: $REPO/dist"
