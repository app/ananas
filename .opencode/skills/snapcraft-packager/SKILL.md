---
name: snapcraft-packager
description: >-
  Package, build or debug the Ananas snap (snapcraft, .snap, core24, layout,
  stage-packages, command-chain, tools/scripts/build-snap*.sh). Use when
  working on snap/snapcraft.yaml, the snap GUI entries, the build container,
  or a failing snap build.
---

# Snapcraft packager for Ananas

Recipe for turning the Ananas monorepo into a single snap named `ananas`.
`snap/snapcraft.yaml` is the source of truth; `tools/README.md` documents the
user-facing build and install steps.

## When to use

Use this skill when the task touches `snap/`, `snap/snapcraft.yaml`,
`snap/gui/`, `snap/local/`, `.snapcraftignore`, or
`tools/scripts/build-snap*.sh`, or when a `.snap` build fails or the installed
snap misbehaves.

## Key facts

- `name: ananas` is **already registered** in the Snap Store (`snapcraft names`
  shows it since 2018-07-25, public), owned by `andrey.paskal`. Do **not** run
  `snapcraft register`. Releases are manual to `latest/edge` for now.
- Base `core24`, `confinement: strict`, `grade: devel`. Channel rules:
  `grade: devel` can be released to `edge` and `beta` only; `candidate` and
  `stable` need `grade: stable`. A plain `snap install ananas` uses `stable`,
  so the final release requires `grade: stable`.
- The build runs in the official OCI image
  `ghcr.io/canonical/snapcraft:8_core24` (Snapcraft 8.11.x on Ubuntu 24.04).
  There is **no host Snapcraft, no LXD and no `--destructive-mode`**: the image
  is the build environment. `podman` is preferred, `docker` also works.
- There is **no generic Qt6 extension** in Snapcraft 8; Qt6 is bundled with
  explicit `build-packages` / `stage-packages`.
- Version is `<VERSION>+git<YYYYMMDD>.<short-sha>` (e.g.
  `0.9.7+git20260924.517477b`), injected as `ANANAS_VERSION` by the build
  scripts and CI; the `adopt-info` part (`craftctl set version`) falls back to
  the plain `VERSION` when Snapcraft is run by hand. Snapcraft derives the
  package file name from it, so date and revision end up in
  `dist/ananas_<version>_<arch>.snap`.
- App commands are namespaced as `<snap>.<app>`: `ananas`,
  `ananas.ananas-designer`, `ananas.ananas-administrator`, `ananas.qdsadm`.
  The `aliases` in `snapcraft.yaml` only *request* short names — the Store does
  **not** grant them automatically (`snap aliases ananas` stays empty), so
  desktop entries must use the dotted command names. Users can add local ones
  with `sudo snap alias ananas.ananas-designer ananas-designer` (and likewise).
- The snap file and the Store *listing* are separate: `title`, `summary`,
  `description`, `license` and the listing icon live in the listing and are
  **not** updated by `snapcraft upload`; push them with
  `snapcraft upload-metadata --force <snap>`.
- Snapcraft 9 is not usable in the container: `snapcraft-rocks` publishes only
  `7_core22`, `8_core22` and `8_core24` images (the `core26-9` branch has no
  published tag). Stay on `8_core24`.

## Hardcoded paths and the layout map

The Ananas binaries use absolute paths that do not exist inside a snap; the
`layout` section maps them back:

| Path                       | Maps to                      | Used for                              |
| -------------------------- | ---------------------------- | ------------------------------------- |
| `/usr/share/ananas`        | `$SNAP/usr/share/ananas`     | schemes, templates, translations      |
| `/usr/lib/ananas`          | `$SNAP/usr/lib/ananas`       | extensions, Qt library path           |
| `/usr/lib/ananas/designer` | `$SNAP/usr/lib/ananas/designer` | plugin and binary RPATH            |
| `/etc/ananas`              | `$SNAP/etc/ananas`           | business-scheme `*.rc`                 |
| `/etc/xdg/ananas`          | `$SNAP/etc/xdg/ananas`       | `ananas.conf` (QSettings, system scope) |
| `/usr/share/X11/xkb`       | `$SNAP/usr/share/X11/xkb`    | XKB keymap data for Qt (`xkb-data`)    |

Do not remove these mappings; they are what keeps the unmodified C++ sources
working. `layout` is incompatible with `classic` confinement — keep `strict`
(or `devmode` while iterating).

## Build

```sh
# From the committed branch (default master; override ANANAS_BRANCH)
bash tools/scripts/build-snap.sh

# From the current working tree (local testing)
bash tools/scripts/build-snap-worktree.sh

# Raw, from the repo root (plain VERSION unless ANANAS_VERSION is passed)
podman run --rm -v "$PWD":/project:z ghcr.io/canonical/snapcraft:8_core24 pack

# Raw with a dated/sha version:
podman run --rm -e ANANAS_VERSION="$(cat VERSION)+git$(date -u +%Y%m%d).$(git rev-parse --short HEAD)" \
  -v "$PWD":/project:z ghcr.io/canonical/snapcraft:8_core24 pack
```

Output: `dist/ananas_<version>_amd64.snap`. Install a local build with
`sudo snap install --dangerous ./dist/ananas_*.snap` (confinement is enforced;
`--devmode` only when debugging confinement). A published build is installed
with `sudo snap install ananas --edge`.

## Package lists (noble / Qt 6.4.2)

- `build-packages`: `build-essential`, `qt6-base-dev`, `qt6-base-dev-tools`,
  `qt6-tools-dev`, `qt6-tools-dev-tools`, `qt6-declarative-dev`,
  `qt6-l10n-tools`, `libmysqlclient-dev`.
- `stage-packages`: `libqt6core6t64`, `libqt6gui6t64`, `libqt6widgets6t64`,
  `libqt6network6t64`, `libqt6xml6t64`, `libqt6printsupport6t64`,
  `libqt6sql6t64`, `libqt6qml6`, `libqt6designer6`,
  `libqt6designercomponents6`, `libqt6sql6-{sqlite,mysql,psql}`,
  `libmysqlclient21`, `libpq5`, `libxkbcommon-x11-0`, `libgl1`,
  `fonts-dejavu-core`, `qt6-wayland`, `qt6-gtk-platformtheme`,
  `yaru-theme-gtk`, `gsettings-desktop-schemas`, `dconf-gsettings-backend`.

Note the `t64` suffixes on the Qt6 runtime packages in noble. The xcb platform
plugin comes from `libqt6gui6t64`; the SQL driver plugins from the
`libqt6sql6-*` packages.

## How the pieces fit

- `snap/local/bin/qt-env` is the `command-chain` for every app. It sets
  `LD_LIBRARY_PATH` (`$SNAP/usr/lib`, the designer dir, `$SNAP/usr/lib/<triplet>`
  and `.../<triplet>/libproxy`) and `QT_PLUGIN_PATH` (the bundled
  `qt6/plugins`). The `libproxy` subdirectory is needed because
  `libproxy.so.1` has an absolute `RUNPATH` that Snapcraft does not rewrite.
  The wrapper deliberately does **not** override `HOME` or `XDG_CONFIG_DIRS`:
  doing so breaks GSettings/dconf and the system theme.
- The wrapper sets `ANANAS_DATA_DIR=$SNAP_USER_COMMON`. The library helpers
  `aDataDir()` / `aInitDataPaths()` (`src/lib/acfg.cpp`) then place the log,
  the default workdir (`~` expansion) and the `QSettings` user scope under that
  directory. Outside a snap the variable is unset and `~/.ananas` is used as
  before, so deb/Windows builds are unaffected.
- Runtime data (schemes, templates, translations, `*.rc`, `ananas.conf`) is
  staged in the `override-build` of the `ananas` part; translations are built
  with `/usr/lib/qt6/bin/lrelease` (not on `PATH`).
- GUI entries live in `snap/gui/<app>.desktop` (named after the app) with the
  icon `snap/gui/<app>.png`:
  - `Exec` must be the snap command `<snap>.<app>` (e.g.
    `ananas.ananas-designer`), not the bare app name. Otherwise snapd cannot
    resolve the app and exports the desktop file **without** `Exec` and
    `X-SnapAppName` (the launcher does nothing and shows a generic icon).
  - `Icon` must be an absolute path via `${SNAP}`, e.g.
    `Icon=${SNAP}/meta/gui/ananas-designer.png`; snapd expands `${SNAP}` when it
    exports the file. A bare `Icon=ananas.png` is left unresolved and shows the
    generic icon.

## Pitfalls

- **Executable bit**: `snap/local/bin/qt-env` must be mode 755, and snapcraft
  caches parts. If you change only permissions, run
  `… snapcraft clean wrappers` (or `clean`) before `pack`.
- **Two icon slots**: the top-level `icon:` (`snap/icon.png`, 256x256) is the
  **Store listing** icon (shown on `snapcraft.io/<name>`, in `snap info` and the
  App Center card); `snap/gui/<app>.png` are the **desktop/app-grid** icons
  referenced by the desktop entries. The listing copy is only refreshed with
  `snapcraft upload-metadata`. Keep the SVG source `snap/icon.svg`.
- **Desktop entry not associated**: `Exec` must be `<snap>.<app>` and `Icon`
  must use `${SNAP}` (see the GUI entries note above); otherwise the launcher
  breaks and the icon is generic.
- **`snapcraft lint` needs LXD** in Snapcraft 8: it fails in the OCI image
  (`unsupported provider 'host'`) and on hosts without LXD. Rely on the linters
  that `snapcraft pack` runs (`classic`, `library`, `metadata`).
- **`libpxbackend-1.0.so: cannot open shared object file`**: `libproxy.so.1`
  (via `libQt6Network`) has an absolute RUNPATH to `.../libproxy/`. Keep the
  `libproxy` subdir in `LD_LIBRARY_PATH` in `qt-env`.
- **System theme ignored (light windows on a dark desktop)**: requires the
  `libqgtk3.so` platform theme (`qt6-gtk-platformtheme`) plus the real `HOME`
  (do not override it, or dconf/gsettings is empty), the themed Yaru/Adwaita
  packages, and the `gsettings` plug. Qt auto-selects gtk3 from
  `XDG_CURRENT_DESKTOP` (e.g. `ubuntu:GNOME`).
- **Wayland**: stage `qt6-wayland`, and rewrite `WAYLAND_DISPLAY` to the
  absolute socket path (inside a snap `XDG_RUNTIME_DIR` is `…/snap.<name>`,
  while the socket is in the parent runtime dir). Even so, native Wayland on
  Qt 6.4 / GNOME has no window decorations and ignores the dark theme, so the
  wrapper prefers **X11/XWayland** when `DISPLAY` is set (`QT_QPA_PLATFORM=xcb`
  if unset). Opt into native Wayland with `QT_QPA_PLATFORM=wayland`.
- **XKB crash on Wayland**: stage `xkb-data`, map `/usr/share/X11/xkb` with
  `layout` and set `XKB_CONFIG_ROOT`; otherwise Qt logs
  `failed to add default include path /usr/share/X11/xkb` and segfaults.
- **Engine command**: because the app name equals the snap name, launch it as
  `ananas`, not `ananas.ananas`. The other commands are
  `ananas.ananas-designer`, `ananas.ananas-administrator` and `ananas.qdsadm`;
  short aliases only exist after `snap alias` (the Store does not grant them
  automatically).
- **`Session management error: Could not open network socket`**: `SESSION_MANAGER`
  points at an unreachable X session manager; the wrapper `unset`s it.
- **Unused-library lint warnings** (mesa/icu/vulkan/dconf) are expected and
  harmless.
- **External LibreOffice/OpenOffice** is unreachable under `strict`
  confinement, so report export that shells out to it will not work; the rest
  of the engine does.
- **MySQL/PostgreSQL over a host Unix socket** are unavailable under `strict`;
  TCP works. The bundled `inventory` scheme uses internal SQLite.
- **User data under `strict`**: `$HOME` and `~/.config` are not reliably
  writable, so the app must not depend on `~/.ananas`. The wrapper sets
  `ANANAS_DATA_DIR=$SNAP_USER_COMMON`; keep it, otherwise the engine cannot
  write its log, workdir or settings.
- **AT-SPI/a11y abort under `strict`**: Qt logs
  `AT-SPI: Error retrieving accessibility bus address` and aborts unless the GUI
  apps carry the `desktop-legacy` plug (it grants access to `org.a11y.Bus`).
- **Stale host artifacts**: `build-snap-worktree.sh` stages a clean copy of the
  tree before mounting it, so `bin/`, `lib/` and `cmake-build/` produced by host
  builds (with RPATHs like `/repo/lib`) cannot break `cmake --install`.
- **Build context**: `.snapcraftignore` keeps VCS/build dirs out of the source
  copy. `.gitignore` lists `.craft/`, `parts/`, `prime/`, `stage/`, `*.snap`.
- `build-snap.sh` uses `git archive`, so files must be committed first; use
  `build-snap-worktree.sh` while iterating.

## Release to the Snap Store (manual)

```sh
bash tools/scripts/build-snap.sh            # from the committed tree
snapcraft upload --release=latest/edge dist/ananas_<version>_amd64.snap
snapcraft status ananas
# The Store listing (title/summary/description/icon) is separate from the snap
# and is NOT updated by `upload`; push it explicitly after changing it:
snapcraft upload-metadata --force dist/ananas_<version>_amd64.snap
```

Then `sudo snap install ananas --edge`. Promote a tested revision later with
`snapcraft release ananas <revision> beta` and finally `... stable`;
`candidate`/`stable` require `grade: stable` in `snapcraft.yaml`. Publishing
from CI (`SNAPCRAFT_STORE_CREDENTIALS`) is not wired up yet.

## Verification

```sh
unsquashfs -l dist/ananas_*.snap | grep -E 'usr/bin/(ananas|qdsadm)|usr/lib/ananas|libQt6DesignerComponents|libqsql|libqxcb'
snap run --shell ananas.qdsadm   # then: ls /usr/share/ananas /usr/lib/ananas /etc/ananas; echo $HOME
```

After install/refresh, an exported desktop entry should be associated and point
at an absolute icon:

```sh
grep -E '^(Exec|Icon|X-SnapAppName)=' \
    /var/lib/snapd/desktop/applications/ananas_ananas-designer.desktop
# Exec=/snap/bin/ananas.ananas-designer
# Icon=/snap/ananas/current/meta/gui/ananas-designer.png
# X-SnapAppName=ananas-designer
```

`snapcraft pack` runs the `classic`, `library` and `metadata` linters; only the
library (unused library) warnings are expected.
