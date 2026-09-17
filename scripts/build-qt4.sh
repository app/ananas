#!/usr/bin/env bash
set -e

WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BRANCH="${ANANAS_BRANCH:-qtscript}"

echo "===> 1. Сборка Podman-образа (Ubuntu 14.04 + Qt4 + QtScript + libqdataschema)..."
podman build -t ananas-qt4-builder -f "$WORKSPACE_DIR/tools/docker/Containerfile.qt4-legacy" "$WORKSPACE_DIR"

echo "===> 2. Сборка ananas-legacy-qt4 (ветка ${BRANCH})..."
podman run --rm \
  -v "$WORKSPACE_DIR":/workspace:z \
  -e BRANCH="$BRANCH" \
  ananas-qt4-builder \
  bash -c '
    set -e
    export CCACHE_DIR=/workspace/tmp/ccache
    mkdir -p "$CCACHE_DIR"
    rm -rf /tmp/ananas
    mkdir -p /tmp/ananas
    git -C /workspace/ananas-legacy-qt4 archive "${BRANCH}" | tar -x -C /tmp/ananas
    cd /tmp/ananas

    # libqdataschema установлена в образ вручную, сообщаем dpkg-shlibdeps её имя пакета
    echo "libqdataschema 1 libqdataschema (>= 1.0.0)" > debian/shlibs.local

    echo "Запуск компиляции..."
    # -d: libqdataschema установлена в образ вручную и не зарегистрирована в dpkg
    dpkg-buildpackage -b -uc -us -d

    echo "Копирование пакетов в /workspace/dist..."
    mkdir -p /workspace/dist
    cp -v /tmp/*.deb /workspace/dist/

    echo "===> ccache stats:"
    ccache -s
  '

echo "===> Сборка завершена успешно! Пакеты: $WORKSPACE_DIR/dist"
