#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"
REPO="$WORKSPACE_DIR/ananas-legacy-qt4"
BRANCH="${ANANAS_BRANCH:-port-qt5}"

# git archive only exports tracked files: fail fast if a source file exists on
# disk but is ignored by .gitignore (it would silently be missing from the build).
MISSING="$(git -C "$REPO" ls-files --others --ignored --exclude-standard \
    | grep -E '\.(h|hpp|cpp|cc|cxx|pro|pri|ui|qrc)$' \
    | grep -vE '/(\.moc|\.obj|\.ui)/' \
    | grep -vE '(^|/)moc_predefs\.h$' \
    | grep -vE '(^|/)(moc_|qrc_)[^/]*\.cpp$' || true)"
if [[ -n "$MISSING" ]]; then
    echo "ERROR: ignored source files would be missing from the build:" >&2
    echo "$MISSING" >&2
    echo "Fix .gitignore or 'git add' them before packaging." >&2
    exit 1
fi

echo "===> 1. Сборка Podman-образа (Ubuntu 24.04 + Qt5 + QtScript + libqdataschema)..."
podman build -t ananas-qt5-builder -f "$SCRIPT_DIR/Containerfile.qt5" "$WORKSPACE_DIR"

echo "===> 2. Сборка ananas-legacy-qt4 (ветка ${BRANCH})..."
podman run --rm \
  -v "$WORKSPACE_DIR":/workspace:z \
  -e BRANCH="$BRANCH" \
  ananas-qt5-builder \
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

    # Runtime SQL drivers are dlopen()ed, so shlibs cannot see them.
    sed -i "s/^Depends: \\(.*\\)libqdataschema$/Depends: \\1libqdataschema, libqt5sql5-sqlite, libqt5sql5-mysql, libqt5sql5-psql/" debian/control

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
