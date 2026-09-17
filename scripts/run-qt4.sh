#!/usr/bin/env bash
# Run the packaged Ananas Qt4 application from the trusty build image.
#
# The .deb targets Ubuntu 14.04 (Qt4) and cannot be installed on a modern host,
# so it is installed inside the container on the fly.
#
# Usage:
#   run-qt4.sh [application]
#
# Environment:
#   ANANAS_IMAGE  build image to use (default: ananas-qt4-builder)
#   ANANAS_APP    application to run (default: ananas-administrator)
#
# If DISPLAY is set the app is started on the host X server (make sure the
# container is allowed, e.g. `xhost +local:`); otherwise it runs under Xvfb.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
IMAGE="${ANANAS_IMAGE:-ananas-qt4-builder}"
APP="${1:-${ANANAS_APP:-ananas-administrator}}"

if ! ls "$WORKSPACE_DIR"/dist/ananas_*.deb >/dev/null 2>&1; then
    echo "No package found in $WORKSPACE_DIR/dist. Run build-qt4.sh first." >&2
    exit 1
fi

podman run --rm -it \
    -v "$WORKSPACE_DIR/dist":/dist:z \
    -v /tmp/.X11-unix:/tmp/.X11-unix:z \
    -e DISPLAY="${DISPLAY:-}" \
    -e APP="$APP" \
    -e QT_X11_NO_MITSHM=1 \
    "$IMAGE" \
    bash -c '
        set -e
        # MIT-SHM is not usable across the container boundary; disable it or the
        # window stays blank with BadAccess/BadDrawable X errors.
        export QT_X11_NO_MITSHM=1
        DEB="$(ls /dist/ananas_*.deb | head -1)"
        # --force-depends: libqdataschema/mysql-client are provided by the image
        dpkg -i --force-depends "$DEB" >/dev/null 2>&1
        if [ -n "${DISPLAY:-}" ]; then
            exec "/usr/bin/$APP"
        else
            exec xvfb-run -a "/usr/bin/$APP"
        fi
    '
