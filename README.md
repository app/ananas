# Ananas legacy build tooling

Containers and scripts used to build the legacy Ananas sources
(`ananas-legacy-qt4`) on a modern host with Podman.

Two build images exist:

- `ananas-qt4-builder` (Ubuntu 14.04, Qt4) — the historical regression bench;
- `ananas-qt5-builder` (Ubuntu 24.04, Qt 5.15) — the current target after the
  Qt4→Qt5 port (see `docs/PORTING.md`).

## Layout

The scripts assume the following workspace layout (they resolve the workspace
root as two levels above this repository):

```
ananas-port/
├── ananas-legacy-qt3/          # Qt3 tree (not built here)
├── ananas-legacy-qt4/          # Qt4 tree (built here)
├── ananas-legacy-qdataschema/  # libqdataschema sources
├── tools/                      # this repository
│   ├── docker/Containerfile.qt4-legacy
│   ├── docker/Containerfile.qt5
│   ├── scripts/build-qt4.sh
│   ├── scripts/build-qt5.sh
│   └── scripts/build-qt5-qdataschema.sh
└── dist/                       # build output (created by the scripts)
```

## Source repositories

The sources are taken from GitHub:

| Directory                   | Origin URL                                   |
| --------------------------- | -------------------------------------------- |
| `ananas-legacy-qt3`         | `https://github.com/app/ananas-labs.git`     |
| `ananas-legacy-qt4`         | `https://github.com/app/ananas-labs-qt4.git` |
| `ananas-legacy-qdataschema` | `https://github.com/app/qdataschema.git`     |

The Qt4 build requires the `origin/qtscript` ref in `ananas-legacy-qt4` and the
`origin/newname` ref in `ananas-legacy-qdataschema`. The Qt5 build uses the
local `port` branch of `ananas-legacy-qt4` and the local `qt5` branch of
`ananas-legacy-qdataschema`.

## Requirements

- `podman` (rootless is fine)
- Network access to `archive.ubuntu.com`, `security.ubuntu.com` and
  `ports.ubuntu.com` (the latter for the Ubuntu 24.04 image)
- Full clones of the repositories listed above, checked out in the layout
  described above, with their `origin` remotes pointing at the URLs above

## Building

Run the whole pipeline (build the image, then compile and package):

```sh
# Qt5 (current)
bash tools/scripts/build-qt5.sh

# Qt4 (legacy regression bench)
bash tools/scripts/build-qt4.sh
```

The resulting Debian package is copied to `dist/`:

```
dist/ananas_0.9.6-1_amd64.deb
```

### Building the libqdataschema package

`ananas` depends on `libqdataschema (>= 1.0.0)`, so to install it on the host
build the matching package first (same image as the main package):

```sh
bash tools/scripts/build-qt5-qdataschema.sh
```

This produces `dist/libqdataschema_1.0.0-1_amd64.deb`. Install both on a Qt5
host:

```sh
sudo apt install ./dist/libqdataschema_1.0.0-1_amd64.deb \
                 ./dist/ananas_0.9.6-1_amd64.deb
```

### Choosing a branch

`build-qt5.sh` builds `port` by default and `build-qt4.sh` builds `qtscript`.
Override with the `ANANAS_BRANCH` environment variable (any local branch or ref
works):

```sh
ANANAS_BRANCH=port bash tools/scripts/build-qt5.sh
ANANAS_BRANCH=port bash tools/scripts/build-qt4.sh
```

`build-qt5-qdataschema.sh` builds the `qt5` branch; override it with
`QDS_BRANCH`:

```sh
QDS_BRANCH=qt5 bash tools/scripts/build-qt5-qdataschema.sh
```

### Building the images only

```sh
podman build --no-cache -t ananas-qt5-builder -f tools/docker/Containerfile.qt5 .
podman build --no-cache -t ananas-qt4-builder -f tools/docker/Containerfile.qt4-legacy .
```

### Running a shell in a build image

```sh
podman run --rm -it -v "$PWD":/workspace:z ananas-qt5-builder bash
podman run --rm -it -v "$PWD":/workspace:z ananas-qt4-builder bash
```

## Porting helpers

Used while porting the codebase (see `docs/PORTING.md`).

```sh
# Read-only burndown report of remaining Qt3Support/QtScript usage
bash tools/scripts/port-metrics.sh

# Build (clean) and run the QtTest suite headlessly under Xvfb
bash tools/scripts/smoke-qt5.sh     # Ubuntu 24.04 / Qt5
bash tools/scripts/smoke-qt4.sh     # Ubuntu 14.04 / Qt4

# Install the built .deb in the matching container and run the application
bash tools/scripts/run-qt5.sh ananas-administrator
bash tools/scripts/run-qt4.sh ananas-administrator
```

The smoke/run scripts take an optional path to `ananas-legacy-qt4` as their
first argument (the run scripts take the application name instead).

### Build caching

The image ships `ccache` (on `PATH` ahead of `gcc`/`g++`). The cache directory
is `tmp/ccache` on the mounted workspace, so it survives `podman run --rm` and
speeds up repeated and clean builds. All build/smoke scripts print `ccache -s`
at the end.

## How it works

Qt5 (`Containerfile.qt5`, Ubuntu 24.04):

1. Installs `qtbase5-dev`, `qttools5-dev` (Designer), `qtscript5-dev`, the Qt5
   SQL drivers and the Debian packaging tools.
2. Builds `libqdataschema` from the `qt5` branch of the
   `ananas-legacy-qdataschema` checkout and installs it under
   `/usr/lib` + the Qt5 header paths.
3. `build-qt5.sh` exports the requested branch with `git archive` (so the host
   working tree is left untouched) and runs `dpkg-buildpackage`.

Qt4 (`Containerfile.qt4-legacy`, Ubuntu 14.04):

1. `Containerfile.qt4-legacy` starts from `ubuntu:14.04`. Since `trusty` has
   been removed from `old-releases.ubuntu.com`, the APT sources point to
   `archive.ubuntu.com` and `security.ubuntu.com`.
2. It installs the Qt4 toolchain (`libqt4-dev`, `qt4-qmake`,
   `libqt4-qt3support`, `libqt4-script`, `libqt4-scripttools`) and the Debian
   packaging tools.
3. It builds and installs `libqdataschema` from the `newname` branch of the
   `ananas-legacy-qdataschema` checkout (the `newname` branch produces
   `libqdataschema.so`, which is what the `qtscript` branch links against).
4. `build-qt4.sh` exports the requested branch with `git archive` (so the host
   working tree is left untouched) and runs `dpkg-buildpackage`.

## Notes

- The default branch is `qtscript`, which ports the scripting layer from QSA
  (Qt Script for Applications) to the Qt4 built-in `QtScript` module. The
  upstream QSA sources required by the `master`/`ubuntu` branches are no longer
  distributed for Qt4, so those branches cannot be built as-is. The bundled
  `qsa-1.1.5` sources are for Qt3 only and do not compile against Qt4.
- `libqdataschema` is installed into the image manually and is not registered
  with `dpkg`, so the build passes `-d` to `dpkg-buildpackage` and a
  `debian/shlibs.local` entry is added to satisfy `dpkg-shlibdeps`.
- The `.containerignore` in the workspace root limits the build context to
  `ananas-legacy-qdataschema`.
- Warnings about the unknown `Description-Ru` field and the missing
  `libananasplugin.so` RPATH are inherited from the project and do not prevent
  the package from being built.
