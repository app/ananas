# Ananas Porting Plan (Qt4 → Qt5/Qt6)

## Context

Legacy codebase: `ananas-legacy-qt4` (~69k LOC C++: 179 `.cpp`, 189 `.h`,
52 `.ui`) plus the external `ananas-legacy-qdataschema` (~4k LOC).

Baseline: the project builds today from the `qtscript` branch in the Ubuntu
14.04 container (`docker/Containerfile.qt4-legacy`,
`scripts/build-qt4.sh`).

Stack: C++ / Qt4 with heavy Qt3Support, QtScript, a vendored Qt Designer fork,
and an external `libqdataschema`.

## Status

- **Phase 0 (infrastructure): done.**
- **Phase 1 (Qt4 without Qt3Support): done.**
  - `Q3*` identifiers, `<q3*.h>` includes, `QT += qt3support` and Q3 widgets in
    `.ui` are all **0** (see `scripts/port-metrics.sh`).
  - `scripts/smoke-qt4.sh`: build ok, `ananas-test` 7/7.
  - `scripts/build-qt4.sh` produces `dist/ananas_0.9.6-1_amd64.deb` with no
    `libqt4-qt3support` dependency.
  - `scripts/run-qt4.sh` runs the packaged app in the trusty container
    (`ananas-administrator` starts and shows the schema/DB dialogs).
  - Branch `port` (from `origin/qtscript`).
- **Phases 2–6: pending.**

## Decisions

- Target: **Qt5/Qt6, C++** (keep the C++/Qt architecture).
- Strategy: **staged** — first make Qt4 build without Qt3Support, then Qt5,
  then Qt6.
- Designer: **excluded** from the first milestone.
- Scripting: migrate QtScript → **QJSEngine**.
- Platforms: **Linux only** (drop Windows packaging for now).

## Principles

1. **Continuously buildable.** The current container is the regression bench;
   `build-qt4.sh` must keep producing a `.deb` after every step.
2. **Burndown metrics.** Track `Q3*`, `<q3*.h>`, `Qt3Support`,
   `QT += qt3support`, and the count of `.ui` files with Q3 widgets.
3. **Branch `port`** from `origin/qtscript`; `qtscript` stays as the rollback
   point.
4. **Preserve the public contract** (`libananas`: `aDatabase`, `aCfg`,
   `aObject`, `aExtension*`; `libananasplugin`: `aEngine`, `aForm`, `aWidget`);
   rewrite internals freely.

## Baseline metrics (start)

- `Q3*` identifiers: 910 in 161 files.
- `#include <q3*.h>`: 143 in 80 files.
- `.ui` files with Q3 widgets: 31.
- `QDesigner*`: 688 in 78 files; vendored Designer: 39 files.
- `QScript*`: 35 in 3 files.

## Phase 0 — Infrastructure

- Create branch `port`; record the baseline build artifact.
- Add `scripts/port-metrics.sh` (read-only burndown report).
- Add a headless smoke harness (`xvfb-run` + `ananas-test`, and
  `ananas-administrator` against a SQLite scheme).
- Add `docs/PORTING.md` (this document) with the Q3→modern mapping table.
- Set up project skills under `.opencode/skill/` and register the
  `qt-documentation-mcp` MCP server (see "Skills and tooling").
- Record licensing (BSD-3-Clause / Qt Commercial + Qt AI Services T&C).

## Phase 1 — Qt4 without Qt3Support (largest)

Stay on Qt4; remove `QT += qt3support` and all `Q3*` code, verifying with the
existing container.

### 1.1 Mechanical replacements

`Q3MainWindow`→`QMainWindow`, `Q3PopupMenu`→`QMenu`, `Q3ToolBar`→`QToolBar`,
`Q3DockWindow`→`QDockWidget`, `Q3WidgetStack`→`QStackedWidget`, `Q3Frame`→
`QFrame`, `Q3GroupBox`/`Q3ButtonGroup`→`QGroupBox`/`QButtonGroup`,
`Q3GridLayout`/`Q3HBoxLayout`/`Q3VBoxLayout`, `Q3FileDialog`→`QFileDialog`,
`Q3Process`→`QProcess`, `Q3TextEdit`→`QTextEdit`, `Q3TextBrowser`→
`QTextBrowser`, `Q3DateEdit`→`QDateEdit`, `Q3ListBox`→`QListWidget`,
`Q3ProgressBar`→`QProgressBar`, `Q3Header`→`QHeaderView`, `Q3Url`→`QUrl`,
`Q3MimeSourceFactory`→`QResource`, `Q3ActionGroup`→`QActionGroup`.
Collections: `Q3ValueList`/`Q3PtrList`/`Q3StrList`→`QList`/`QStringList`,
`Q3Dict`/`Q3IntDict`→`QHash`, `Q3TextStream`→`QTextStream`.
Rich text/printing: `Q3SimpleRichText`→`QTextDocument`,
`Q3PaintDeviceMetrics`→`QFontMetrics`.

### 1.2 Trees/lists/tables

- `Q3ListView`/`Q3ListViewItem` (170) → `QTreeWidget`/`QTreeWidgetItem` (or
  `QAbstractItemModel` when virtualization is required).
- `Q3Table`/`Q3DataTable`/`Q3CheckTableItem` (85) → `QTableWidget`/
  `QTableWidgetItem`.
- `Q3IconView`/`Q3IconViewItem`, `Q3ScrollView`→`QScrollArea`.
- Keep the public `aWidget`/`aObject` methods stable for scripts.

### 1.3 Database layer (key risk)

- `aDataTable : Q3SqlCursor` → rewrite on `QSqlQuery` + `QSqlTableModel`,
  preserving `value/setValue/select/New/Update/calcFieldValue`.
- `Q3SqlForm`/`Q3SqlPropertyMap`/`Q3SqlEditorFactory` → custom field↔widget
  binding + `QStyledItemDelegate`.
- `Q3SqlSelectCursor`/`Q3SqlFieldInfo` → `QSqlQuery`/`QSqlField`.
- Fix the PostgreSQL driver-name bug (`QPOSTGRESQL` vs `QPSQL7`).

### 1.4 `.ui` conversion

- Replace Q3 widget classes in 31 `.ui` files with modern equivalents;
  regenerate with `uic`; verify by building.

### 1.5 Cleanup

- Drop `QT += qt3support` (`ananas.pri`, `aextsql.pro`) and `-L/usr/X11R6/lib/`.
- Delete the dead parallel metadata layer `src/lib/metadata/AMeta*`.
- Remove QSA leftovers (`-lqsa` in `test.pro`, `QSA_NO_IDE`, `qsa-1.1.5`).

**Acceptance:** zero `Q3*`/`<q3*.h>`/`Qt3Support`/`qt3support`; `.ui` free of Q3;
package builds in the trusty container; `ananas-test` green; smoke
`ananas-administrator` on SQLite.

## Phase 2 — Qt4 → Qt5

- `QT += widgets printsupport`; `qtestlib`→`testlib`; `QFormBuilder` from
  `QtUiTools` (`QT += uitools`); widget plugins use `QT += designer`.
- `Q_EXPORT_PLUGIN2` → `Q_PLUGIN_METADATA` (`aWidgetsCollection`,
  `AExtensionPlugin`, `A_EXPORT_PLUGIN`).
- Removed/changed APIs: `QWorkspace`→`QMdiArea`,
  `QApplication::setMainWidget` (drop), `QApplication::desktop()`→`QScreen`,
  `setCaption`→`setWindowTitle`, `QDir::convertSeparators`→
  `toNativeSeparators`, `QString::utf8`→`toUtf8`,
  `QTextCodec::setCodecForCStrings` (drop), `QRegExp`→`QRegularExpression`,
  `QFontMetrics::width`→`horizontalAdvance`, `QAssistantClient` (drop),
  `QLibraryInfo::location`→`path`.
- Migrate the old `QSettings` API (~50 sites) to `value`/`setValue`.
- Move hardcoded `/usr/lib/ananas`, `/usr/share/ananas` to `QStandardPaths`.
- Add `docker/Containerfile.qt5` on a current LTS + `scripts/build-qt5.sh`.
- Build `libqdataschema` for Qt5 in parallel.

## Phase 3 — QtScript → QJSEngine

- `aEngine`/`aForm`/`mainform`: `QScriptEngine`→`QJSEngine`,
  `QScriptValue(List)`→`QJSValue(List)`, `newQMetaObject` constructors → JS
  shim/`QJSValue` callable, `uncaughtException`→`QJSValue::isError`.
- `QScriptEngineDebugger` has no equivalent — remove/replace the debugger.
- Remove `Q_PROPERTY(... SCRIPTABLE true)` (dead layer already deleted).
- Verify by executing the sample schemes (`applications/inventory`,
  `applications/money`).

## Phase 4 — Qt5 → Qt6

- `QTextCodec`→`QStringConverter`/`Qt5Compat`; `Qt::SplitBehavior`;
  `QLibraryInfo`; drop `QDesktopWidget`; `QAction` in QtGui; `qmake`→CMake.
- Port `libqdataschema` to Qt6 (drivers, `QSqlTableModel` aliases).
- Add `docker/Containerfile.qt6` + `scripts/build-qt6.sh`.

## Phase 5 — Build and packaging

- Migrate qmake → CMake: targets `libananas`, `ananasplugin`, `ananas`,
  `ananas-administrator`, extensions, tests.
- Update `debian/` (Qt5/6 deps, `libqdataschema`); drop the outdated
  `.spec`/`.iss` packaging.
- Remove the non-existent `doc/doc.pro` from the install rule.

## Phase 6 — Designer (deferred, out of scope)

- Options: (a) metadata editor + stock Qt Designer with our widget plugins;
  (b) rebase the vendored Designer fork onto Qt5/6 sources. Revisit after the
  Qt5/6 runtime is done.

## Skills and tooling by phase

| Skill                        | Phase                 | Purpose                                                            |
| ---------------------------- | --------------------- | ------------------------------------------------------------------ |
| `qt-documentation-mcp` (MCP) | 0 (setup) → 2–4 (use) | Look up current Qt API during Qt4→Qt5→Qt6 replacements             |
| `qt-cpp-review`              | 4                     | Lint (60+ rules) + ownership/API/threading review; pre-commit gate |
| `qt-cpp-docs`                | 4                     | Generate Markdown docs for `libananas`/`libananasplugin`           |
| `qt-cmake-project`           | 5                     | Correct Qt6 CMake; avoid `qt5_*`/qmake-isms                        |
| `qt-ui-design`               | 6 (optional)          | GUI audit if the designer returns                                  |

Installation notes: vendor skills into `.opencode/skill/<name>/` (keep
`LICENSE`/attribution); register the MCP server in the opencode config.
Install each skill at the start of its phase, not upfront.

Source of the skills: <https://github.com/TheQtCompanyRnD/agent-skills>
(BSD-3-Clause / Qt Commercial; subject to the Qt AI Services T&C).

## Verification

1. Full container build + `dpkg-buildpackage` → `.deb`.
2. `ananas-test` (QtTest) + burndown metrics.
3. Headless smoke: `xvfb-run ananas-administrator` on SQLite + demo scheme;
   after Phase 3, execute the `inventory`/`money` scripts.
4. `scripts/port-check.sh` chaining steps 1–3.

## Risks

- `Q3Sql*` layer is a rewrite, not a rename; keep the public script API.
- `libqdataschema` is an external blocker; port in lockstep (or vendor it as
  `src/qdataschema`).
- Loss of the script debugger when moving to `QJSEngine`.
- UX regressions when replacing `Q3ListView`/`Q3Table` with item widgets.
- Phase 1 is the largest; split by workstreams 1.1→1.5 with per-step checks.

## Out of scope

- QML (not used by Ananas).
- Windows/embedded packaging.
- The form designer (Phase 6, deferred).
