# Archived Qt4 / Qt5 tooling

The active tooling targets **Qt6** (see the parent `README.md` and
`../docs/PORTING.md`). The Qt4 and Qt5 build images and scripts are kept here
for history and for reproducing the last state that built under Qt5.

| Directory | Purpose |
| --------- | ------- |
| `qt4/`    | Ubuntu 14.04 + Qt4 (the original baseline) |
| `qt5/`    | Ubuntu 24.04 + Qt 5.15 (the intermediate port) |

The scripts had to be moved one directory deeper, so their workspace path
resolution was adjusted (`WORKSPACE_DIR` is now three levels above the script
and the Containerfile lives next to it).

## Reproducing the Qt5 build

The last Qt6/Qt5-dual state of `ananas-legacy-qt4` is on branch **`port-qt5`**.
Build it with the archived Qt5 tooling:

```sh
# from the workspace root
git -C ananas-legacy-qt4 checkout port-qt5

bash tools/archive/qt5/build-qt5.sh            # image + ananas .deb
bash tools/archive/qt5/build-qt5-qdataschema.sh # libqdataschema .deb

# or build + test in place
bash tools/archive/qt5/smoke-qt5.sh
```

`build-qt5.sh` defaults to `ANANAS_BRANCH=port-qt5`; the qdataschema script uses
the `qt5` branch of `ananas-legacy-qdataschema`. `run-qt5.sh` installs the
resulting `.deb` in the Qt5 image.

## Reproducing the Qt4 build

The Qt4 baseline is the `qtscript` branch:

```sh
git -C ananas-legacy-qt4 checkout qtscript
bash tools/archive/qt4/build-qt4.sh
bash tools/archive/qt4/smoke-qt4.sh
```

## Note

These scripts are not maintained anymore and are not part of the Qt6
verification pipeline.
