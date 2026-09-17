# Ananas legacy build tooling

Containers and scripts used to build the Ananas sources
(`ananas-legacy-qt4`) on a modern host with Podman.

The active target is **Qt6** (`ananas-qt6-builder`, Ubuntu 24.04 + Qt 6.4, see
`docs/PORTING.md`). The Qt4 and Qt5 tooling is kept under `archive/` for
history.

## Layout

The scripts assume the following workspace layout (they resolve the workspace
root as two levels above this repository):

```
ananas-port/
├── ananas-legacy-qt3/          # Qt3 tree (not built here)
├── ananas-legacy-qt4/          # sources (built here)
├── ananas-legacy-qdataschema/  # libqdataschema sources
├── tools/                      # this repository
│   ├── docker/Containerfile.qt6
│   ├── scripts/build-qt6.sh
│   ├── scripts/build-qt6-qdataschema.sh
│   ├── scripts/smoke-qt6.sh
│   ├── scripts/run-qt6.sh
│   ├── scripts/port-metrics.sh
│   └── archive/                # historical Qt4/Qt5 tooling (see archive/README.md)
└── dist/                       # build output (created by the scripts)
```

## Source repositories

The sources are taken from GitHub:

| Directory                   | Origin URL                                   |
| --------------------------- | -------------------------------------------- |
| `ananas-legacy-qt3`         | `https://github.com/app/ananas-labs.git`     |
| `ananas-legacy-qt4`         | `https://github.com/app/ananas-labs-qt4.git` |
| `ananas-legacy-qdataschema` | `https://github.com/app/qdataschema.git`     |

The Qt6 build uses the local `port` branch of `ananas-legacy-qt4` and the local
`qt6` branch of `ananas-legacy-qdataschema`. The last state that also built
under Qt5 is kept on `ananas-legacy-qt4` branch `port-qt5` (qdataschema `qt5`);
the original Qt4 baseline is `qtscript` (qdataschema `newname`).

## Requirements

- `podman` (rootless is fine)
- Network access to `archive.ubuntu.com`, `security.ubuntu.com` and
  `ports.ubuntu.com` (the latter for the Ubuntu 24.04 image)
- Full clones of the repositories listed above, checked out in the layout
  described above, with their `origin` remotes pointing at the URLs above

## Building

Run the whole pipeline (build the image, then compile and package):

```sh
bash tools/scripts/build-qt6.sh
```

The resulting Debian package is copied to `dist/`:

```
dist/ananas_0.9.6-1_amd64.deb
```

### Building the libqdataschema package

`ananas` depends on `libqdataschema (>= 1.0.0)`, so to install it on the host
build the matching package first (same image as the main package):

```sh
bash tools/scripts/build-qt6-qdataschema.sh
```

This produces `dist/libqdataschema_1.0.0-1_amd64.deb`. Install both on a Qt6
host:

```sh
sudo apt install ./dist/libqdataschema_1.0.0-1_amd64.deb \
                 ./dist/ananas_0.9.6-1_amd64.deb
```

### Choosing a branch

`build-qt6.sh` builds `port` by default. Override with the `ANANAS_BRANCH`
environment variable (any local branch or ref works):

```sh
ANANAS_BRANCH=port bash tools/scripts/build-qt6.sh
```

`build-qt6-qdataschema.sh` builds the `qt6` branch; override with `QDS_BRANCH`:

```sh
QDS_BRANCH=qt6 bash tools/scripts/build-qt6-qdataschema.sh
```

### Building the image only

```sh
podman build --no-cache -t ananas-qt6-builder -f tools/docker/Containerfile.qt6 .
```

### Running a shell in the build image

```sh
podman run --rm -it -v "$PWD":/workspace:z ananas-qt6-builder bash
```

## Porting helpers

Used while porting the codebase (see `docs/PORTING.md`).

```sh
# Read-only burndown report of remaining Qt3Support/QtScript usage
bash tools/scripts/port-metrics.sh

# Build (clean) and run the QtTest suite headlessly under Xvfb
bash tools/scripts/smoke-qt6.sh

# Install the built .deb in the image and run the application
bash tools/scripts/run-qt6.sh ananas-administrator
```

The smoke script takes an optional path to `ananas-legacy-qt4` as its first
argument; the run script takes the application name instead.

### Build caching

The image ships `ccache` (on `PATH` ahead of `gcc`/`g++`). The cache directory
is `tmp/ccache` on the mounted workspace, so it survives `podman run --rm` and
speeds up repeated and clean builds. All build/smoke scripts print `ccache -s`
at the end.

## How it works

Qt6 (`Containerfile.qt6`, Ubuntu 24.04):

1. Installs `qt6-base-dev`, `qt6-tools-dev` (Designer), `qt6-declarative-dev`
   (QtQml/QJSEngine), the Qt6 SQL drivers and the Debian packaging tools;
   symlinks `qmake`/`lrelease` to their Qt6 locations.
2. Builds `libqdataschema` from the `qt6` branch of the
   `ananas-legacy-qdataschema` checkout and installs it under
   `/usr/lib` + the Qt6 header paths.
3. `build-qt6.sh` exports the requested branch with `git archive` (so the host
   working tree is left untouched) and runs `dpkg-buildpackage`.

Historical Qt4/Qt5 tooling lives in `archive/`; see `archive/README.md` for how
to reproduce the `port-qt5` and `qtscript` builds.

## Notes

- `libqdataschema` is installed into the image manually and is not registered
  with `dpkg`, so the build passes `-d` to `dpkg-buildpackage` and a
  `debian/shlibs.local` entry is added to satisfy `dpkg-shlibdeps`.
- The `.containerignore` in the workspace root limits the build context to
  `ananas-legacy-qdataschema`.
- The unknown `Description-Ru` control field is inherited from the project and
  only produces a warning.
- The app binaries carry an RPATH to `/usr/lib/ananas/designer` (where
  `libananasplugin.so` is installed), so the `ananas` engine starts without
  `LD_LIBRARY_PATH`.
- The vendored Designer (`src/designer`) is out of scope (Phase 6) and is not
  built.
