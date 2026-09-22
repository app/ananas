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
├── tools/                  # this directory
│   ├── docker/Containerfile
│   └── scripts/
│       ├── build-deb.sh            # .deb from the committed tree
│       ├── build-deb-worktree.sh   # .deb from the working tree (local testing)
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
- Distribution packaging beyond Debian/Ubuntu (`build/rpm`, `build/win32`) is
  not maintained yet.
