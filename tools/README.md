# Ananas build tooling

Containers and scripts used to build, test and package the Ananas monorepo on a
modern host with Podman or Docker. Everything is built with **CMake** on
**Qt6** (Ubuntu 24.04 + Qt 6.4); there is no qmake build.

The repository contains the whole system: the engine, the administrator, the
designer, the libraries and the bundled `src/qdataschema`.

## Quick start

After cloning, build and test the whole tree in the container — no host
Qt6/CMake toolchain is needed:

```sh
bash tools/scripts/smoke.sh
```

To build the Debian packages instead (written to `dist/`):

```sh
bash tools/scripts/build-deb.sh
```

The first run builds the `ananas-qt6-builder` image; later runs reuse it.

## Layout

```
ananas/
├── src/                    # engine, lib, plugins, designer, extensions, test
│   └── qdataschema/        # libqdataschema (separate .deb)
├── applications/           # inventory business scheme
├── build/                  # packaging (ubuntu/debian, rpm, inno, menus)
├── snap/                   # Snapcraft manifest, GUI entries, wrappers
│   ├── snapcraft.yaml
│   ├── gui/                # .desktop files and icons
│   └── local/bin/qt-env    # command-chain wrapper for all apps
├── tools/                  # this directory
│   ├── docker/Containerfile
│   └── scripts/
│       ├── build-deb.sh            # .deb from the committed tree
│       ├── build-deb-worktree.sh   # .deb from the working tree (local testing)
│       ├── build-snap.sh           # .snap from the committed tree
│       ├── build-snap-worktree.sh  # .snap from the working tree
│       ├── smoke.sh                # CMake build + ananas-test (Xvfb)
│       ├── smoke-designer.sh       # designer wrapper + app start (Xvfb)
│       └── extract-cfg-form.py     # test fixture for smoke-designer.sh
└── dist/                   # package output (created by the scripts)
```

## Requirements

- `podman` (recommended, rootless is fine) or `docker`. If both are installed,
  Podman is used; force the other one with `CONTAINER=docker bash …`.
- Network access to `archive.ubuntu.com`, `security.ubuntu.com` and
  `ports.ubuntu.com` (for the Ubuntu 24.04 image)
- Network access to `ghcr.io` to pull the Snapcraft OCI image for `.snap` builds

## Building and testing

```sh
# Build the whole tree with CMake and run the test suite under Xvfb.
bash tools/scripts/smoke.sh

# Designer checks: form open/save round-trip and application start.
bash tools/scripts/smoke-designer.sh
```

Both scripts build the `ananas-qt6-builder` image (cached), configure and build
with CMake, and run the checks headlessly.

### Debian packages

Two binary packages are produced from the single source tree:
`ananas` (engine, administrator, designer, inventory scheme) and
`libqdataschema` (library, headers, `qdsadm`).

```sh
# From the committed branch (default: master)
bash tools/scripts/build-deb.sh

# From the current working tree (for testing uncommitted changes)
bash tools/scripts/build-deb-worktree.sh
```

The packages are written to `dist/`:

```sh
sudo apt install ./dist/libqdataschema_*_amd64.deb ./dist/ananas_*_amd64.deb
```

### Snap package

A single snap named `ananas` bundles the engine, the administrator, the
designer and `qdsadm`. It is built inside the official Snapcraft OCI image
(`core24`), so no host Snapcraft, LXD or `snapcraft --destructive-mode` is
needed — the image is the build environment:

```sh
# From the committed branch (default: master)
bash tools/scripts/build-snap.sh

# From the current working tree
bash tools/scripts/build-snap-worktree.sh
```

The snap is written to `dist/` and can be installed in developer mode:

```sh
sudo snap install --devmode --dangerous ./dist/ananas_*.snap
```

Its version is `<VERSION>+git<YYYYMMDD>.<short-sha>` (for example
`0.9.7+git20260924.517477b`), so the build date and the git revision show up in
`snap info ananas` and in the package file name. The apps are exposed as
`ananas`, `ananas-designer`, `ananas-administrator` and `qdsadm` (declared as
aliases in `snapcraft.yaml`); a local install may need them enabled with
`sudo snap alias ananas.ananas-designer ananas-designer` and so on.

The Ananas binaries use a few absolute paths (`/usr/share/ananas`,
`/usr/lib/ananas`, `/etc/ananas`); the `layout` section in
`snap/snapcraft.yaml` maps them back into the snap. A `command-chain` wrapper
(`snap/local/bin/qt-env`) points the loader and Qt at the bundled libraries and
plugins. The name `ananas` is already registered in the Snap Store, so
`name: ananas` is used as-is (do not run `snapcraft register`).

### Choosing a branch

`build-deb.sh` builds `master` by default; override with `ANANAS_BRANCH`:

```sh
ANANAS_BRANCH=master bash tools/scripts/build-deb.sh
```

### Building the image only

```sh
podman build -t ananas-qt6-builder -f tools/docker/Containerfile .
```

### Running a shell in the build image

```sh
podman run --rm -it -v "$PWD":/repo:z ananas-qt6-builder bash
```

## Build caching

The image ships `ccache` (on `PATH` ahead of `gcc`/`g++`). The cache directory is
`/tmp/ccache` inside the container; the scripts print `ccache -s` at the end.

## Notes

- The app binaries carry an RPATH to `/usr/lib/ananas/designer` (where
  `libananasplugin.so` is installed), so the engine starts without
  `LD_LIBRARY_PATH`.
- The vendored Qt4 Designer fork was replaced by a wrapper over the public Qt6
  Designer API; the legacy fork is kept in the archived `ananas-labs-qt4`
  repository.
- Snap packaging is supported (`snap/snapcraft.yaml`, `tools/scripts/build-snap*.sh`).
  Other distribution packaging (`build/rpm`, `build/win32`) is not maintained yet.
