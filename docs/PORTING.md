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
- **Phase 2 (Qt4 → Qt5): done.**
  - `docker/Containerfile.qt5` (Ubuntu 24.04 + Qt 5.15) and
    `scripts/build-qt5.sh` produce `dist/ananas_0.9.6-1_amd64.deb` with Qt5
    dependencies only (no Qt4 / Qt3Support). The app binaries carry an RPATH to
    `/usr/lib/ananas/designer` (`libananasplugin.so`).
  - `libqdataschema` is ported to Qt5 on the `qt5` branch of
    `ananas-legacy-qdataschema` and installed into the image.
  - `scripts/smoke-qt5.sh`: clean Qt5 build + `ananas-test` 7/7.
  - `scripts/run-qt5.sh` installs the package in the image; the packaged
    `ananas-administrator` starts (manual GUI smoke pending).
  - `scripts/build-qt5-qdataschema.sh` builds the matching
    `dist/libqdataschema_1.0.0-1_amd64.deb` (Qt5), so both packages install
    together on a Qt5 host.
- **Phase 3 (QtScript → QJSEngine): done.**
  - `aEngine::code` is a `QJSEngine`; the script debugger was dropped.
  - Build + `ananas-test` 7/7; an integration harness exercised the engine
    against the `applications/inventory` scheme (SQLite): slot binding via
    `newQObject`, global function call, `QVariant` bridging and `isError()`
    detection all pass.
  - Phase 3b restored the QSA script API (object constructors, form-method
    context, `print`) that business-scheme scripts rely on.
- **Phase 4 (Qt5 → Qt6): done.**
  - `docker/Containerfile.qt6` (Ubuntu 24.04 + Qt 6.4) and `scripts/build-qt6.sh`
    produce a Qt6-only `dist/ananas_0.9.6-1_amd64.deb`; the SQL driver packages
    are declared in `debian/control`.
  - `libqdataschema` is ported to Qt6 on the `qt6` branch
    (`scripts/build-qt6-qdataschema.sh`).
  - `scripts/smoke-qt6.sh`: clean Qt6 build + `ananas-test` 7/7; the scripting
    integration harness passes on the inventory scheme under Qt 6.4.2.
  - `qmake` still drives the build (`qmake6`); CMake is Phase 5.
- **Qt6-only cleanup: done.**
  - `QT_VERSION` guards were removed from `ananas-legacy-qt4` and
    `ananas-legacy-qdataschema`; the code now compiles only against Qt6.
  - The last dual Qt5/Qt6-buildable state is kept on `ananas-legacy-qt4`
    branch `port-qt5` (qdataschema `qt5`); Qt4/Qt5 tooling moved to
    `tools/archive/`.
- **Phase 5 (CMake migration): done.** `CMakeLists.txt` + per-directory targets
  build and install the whole tree on Qt6; `ananas-test` 7/7. Packaging
  (`debian/`, `build/*`) is deferred and untouched.
- **Phase 6: pending (out of scope).**

## Handoff (next session)

- Branch: `ananas-legacy-qt4` @ `port` (Qt6-only, qmake + CMake). History:
  `port-qt5` (last Qt5/Qt6 dual state), `qtscript` (untouched Qt4 baseline),
  `port` @ `b2f77d0` (Phase 1 result). Tooling lives in the `tools` repo
  (`main`), sources in `ananas-legacy-qt4` / `ananas-legacy-qdataschema`
  (branch `qt6`).
- Phases 1–5 are done; the next step is packaging (deferred): update `debian/`
  and the legacy `build/*` (RPM/Inno/menus) for Qt6/CMake, or add CPack. The
  legacy scripts are kept, not deleted.
- Commands:
  - burndown: `bash tools/scripts/port-metrics.sh`
  - Qt6 build + tests: `bash tools/scripts/smoke-qt6.sh`
  - package: `ANANAS_BRANCH=port bash tools/scripts/build-qt6.sh`
  - package `libqdataschema` (ananas dependency):
    `bash tools/scripts/build-qt6-qdataschema.sh`
  - run the app: `bash tools/scripts/run-qt6.sh ananas-administrator`
  - historical Qt4/Qt5 builds: `tools/archive/` (see `archive/README.md`)
- Image: `ananas-qt6-builder` = Ubuntu 24.04 + Qt 6.4 + QtQml (QJSEngine) +
  `libqdataschema` (`ananas-legacy-qdataschema@qt6`); ccache at
  `<workspace>/tmp/ccache`.
- Known caveat: the rewritten `wDBTable`/`wTable`/`awidget` have no functional
  tests; only the schema/DB dialogs were smoke-tested manually.
- The Qt6 `.deb` targets Ubuntu 24.04.

## Decisions

- Target: **Qt6, C++** (keep the C++/Qt architecture). Qt5 was a stepping
  stone; the last dual-buildable state is kept on branch `port-qt5` and the
  Qt4/Qt5 tooling under `tools/archive/`, but neither is maintained.
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

## Phase 2 — Qt4 → Qt5 (done)

Reference: <https://wiki.qt.io/Transition_from_Qt_4.x_to_Qt5>. The counts below
were measured on the `port` branch at the end of Phase 1.

Outcome: `port` builds and links against Qt 5.15, `ananas-test` is 7/7 and
`build-qt5.sh` produces a Qt5-only `.deb`. `QRegExp` and `QTextCodec` were left
in place on purpose (still available in Qt5, removed in Qt6 — Phase 4). The
vendored Designer (`src/designer`) is still Qt4 and out of scope (Phase 6).

### 2.1 Build system

- `QT += widgets printsupport` (QFormBuilder lives in `QtDesigner`, so the
  widget plugins add `QT += designer`; `uitools` turned out to be unnecessary).
- `CONFIG += qtestlib` → `QT += testlib` (`src/test/test.pro`).
- Added `docker/Containerfile.qt5` (Ubuntu 24.04 LTS) + `scripts/build-qt5.sh`,
  `scripts/smoke-qt5.sh`, `scripts/run-qt5.sh`.
- Built `libqdataschema` for Qt5 in lockstep (branch `qt5`): `QChar::toAscii`,
  `QString::null`, driver names `QMYSQL3`/`QPSQL7` → `QMYSQL`/`QPSQL`, and
  modernized its Debian packaging (compat 9, Qt5 paths, no doxygen docs) so
  `build-qt5-qdataschema.sh` emits an installable `libqdataschema` `.deb`.

### 2.2 Includes

- Split `<QtGui/…>`: widget classes (`QAction`, `QApplication`, `QDialog`,
  `QGridLayout`, `QHeaderView`, `QLabel`, `QMenu`, `QPushButton`,
  `QTreeWidget`, `QTableWidget`, …) → `<QtWidgets/…>`; painting/event classes
  (`QIcon`, `QPainter`, `QCloseEvent`, `QTextBlock`, `QTextCharFormat`,
  `QTextCursor`, `QTextDocument`, `QSyntaxHighlighter`) stay `<QtGui/…>`.
  Automate with `fixqt4headers.pl` (qtbase/bin) if available.
- `QFormBuilder` → `<QtDesigner/QFormBuilder>` (`src/plugins/aform.cpp`); in
  Qt5 it is part of the Designer module, not QtUiTools.
- `QPrinter`/`QPrintDialog` → `<QtPrintSupport/…>`
  (`src/lib/report/areport.cpp`).

### 2.3 API replacements (measured)

| Qt4 | Qt5 | Sites |
| --- | --- | --- |
| `QWorkspace` | `QMdiArea` | 7 |
| `QDir::convertSeparators` | `toNativeSeparators` | 58 |
| `QRegExp` | `QRegularExpression` | 10 |
| `QTextCodec` / `setCodecForCStrings` | remove / UTF-8 default | 6 / 2 |
| `QHeaderView::setResizeMode` | `setSectionResizeMode` | 2 |
| `qInstallMsgHandler` | `qInstallMessageHandler` | 1 |
| `Q_EXPORT_PLUGIN2` | `Q_PLUGIN_METADATA` | 2 |
| `Qt::WFlags` | `Qt::WindowFlags` | 34 |
| `TRUE` / `FALSE` | `true` / `false` | 43 |
| `QString::null` | `QString()` | 110 |
| `QWidget::setShown` | `setVisible` | 4 |
| `QFileDialog::setFilter` | `setNameFilter` | 1 |

Already zero: `QDesktopWidget`, `QApplication::desktop()`,
`QApplication::setMainWidget`, `QLibraryInfo`, `QDesktopServices`,
`QUrl::addQueryItem`, `QDrag`, `QWeakPointer`, QtConcurrent, QtWebKit,
`UnicodeUTF8` (only comments). Old `QSettings` API was migrated during Phase 1.

### 2.4 Plugins

- `aWidgetsCollection` got `Q_PLUGIN_METADATA(IID …)` in
  `src/plugins/awidgets_plugin.h`.
- The `A_EXPORT_PLUGIN` macro was dropped: `Q_PLUGIN_METADATA` must be visible
  to `moc`, which does not expand macros. Each `src/extensions/*/*.cpp` now
  declares a concrete plugin class deriving from `AExtensionPlugin<type>` with
  `Q_OBJECT` + `Q_PLUGIN_METADATA` and includes its generated `<file>.moc`.

### 2.5 Scripting

- QtScript was left in place during Phase 2 and migrated in Phase 3
  (`QJSEngine`).

### 2.6 Verification

- Done: `scripts/build-qt5.sh` (image + `.deb`) and `scripts/smoke-qt5.sh`
  (clean build + `ananas-test` 7/7). `scripts/run-qt5.sh` starts the packaged
  `ananas-administrator`; the manual GUI smoke (schema/DB dialogs) is pending.

Note: this phase is Qt4→Qt5 only; the Qt5→Qt6 items (`QTextCodec`, `QRegExp`,
`Qt::SplitBehavior`, `QAction` in QtGui, qmake→CMake) stay in Phase 4.

## Phase 3 — QtScript → QJSEngine (done)

- `aEngine::code`: `QScriptEngine`→`QJSEngine`; `QScriptValue(List)`→
  `QJSValue(List)`; `QScriptEngineDebugger` removed (no QJSEngine equivalent).
- Property existence is checked with `QJSValue::isCallable()`; global calls use
  `QJSValue::call(args)` (no `thisObject` overload); `newVariant()` →
  `QJSEngine::toScriptValue()`.
- `uncaughtException()` → `QJSValue::isError()`: `aEngine::checkScriptError()`
  logs the result of `evaluate()` (global module, actions); `aForm` logs its
  form module errors.
- `aObjectsFactory` is kept for plugin API compatibility; its constructor now
  registers meta objects via `QJSEngine::newQMetaObject()` (the QtScript
  native-function constructor hook has no QJSEngine equivalent). Its `create()`
  factory is unchanged.
- `QT += script scripttools` → `QT += qml`; the image/control use
  `qtdeclarative5-dev`.
- Verified: build + `ananas-test` 7/7, and an integration harness that opened
  `applications/inventory` (SQLite) through `aDatabase`/`aEngine` — slot binding,
  global function calls, `QVariant` bridging and `isError()` all pass.
- Compatibility note: `aEngine::code` changes type, a source-level break for any
  plugin touching it (none in-tree; Qt4 ABI is incompatible anyway).

### Phase 3b — restore the QSA script API (done)

The Qt3→Qt4 port replaced QSA with QtScript but dropped the QSA execution
environment, so business-scheme scripts (`<sourcecode>` in the `.cfg`) no
longer ran. Restored on top of QJSEngine (core, transparent to schemes):

- Object constructors: `aEngine` owns an `aObjectsFactory` and installs JS
  constructor shims (`new Document(...)`, `new Catalogue(...)`,
  `new Report(...)`, `new ARegister(...)`, extension classes such as
  `new Service()`/`new SQL()`) calling
  `sys.createObject()` → `aObjectsFactory::create()`.
- Form context: `aForm` sets itself as `__ananas_form`; global wrappers for the
  `aForm` slots (`Value`, `SetValue`, `TabValue`, `Widget`, `Propis`, ...)
  dispatch to it, so form modules and the global-module helpers resolve the
  form methods.
- `print()` writes to the message window and stdout.
- Fixed `AExtensionFactoryPrivate` crashing on an empty extension directory
  (exposed by calling `keys()`).

Verified: shims present, `new Catalogue()` returns a QObject, a real document
form module loads and `on_formstart` runs without `ReferenceError`.

## Phase 4 — Qt5 → Qt6 (done)

- `QRegExp`/`QRegExpValidator` → `QRegularExpression`/
  `QRegularExpressionValidator` (`setMinimal(true)` →
  `InvertedGreedinessOption`, `indexIn`/`matchedLength` → match objects).
- `QTextStream::setCodec` → `setEncoding(QStringConverter::Utf8)`; `QTextCodec`
  dropped (the text extension maps `QStringConverter` encodings).
- `Qt::WindowFlags` defaults `0` → `Qt::WindowFlags()` (Qt6 makes the int
  conversion an error).
- `QString::sprintf` → `arg()`, `trUtf8` → `tr`,
  `QDateTime::toTime_t` → `toSecsSinceEpoch`.
- `QTime` stopwatch → `QElapsedTimer` (`aTime`).
- `QMap::insertMulti` → `QMultiMap` (`AMetaObject` children).
- `QSqlField::type()` compared against `QMetaType` (Qt6) / `QVariant` (Qt5).
- `QComboBox::setAutoCompletion`/`autoCompletion` reimplemented via
  `QCompleter`.
- `QPrinter::pageRect()` → `pageLayout().paintRectPixels()`;
  `QPalette::Background` → `Window`; `QShortcut` moved to QtGui;
  `qBinaryFind` → `std::lower_bound`.
- Qt6 moc emits a default-constructor metatype for every `Q_OBJECT` class, so
  the declared-but-undefined `aForm()`/`aRole()`/`aUser()` ctors are now
  defined.
- `uic` on Qt6 defaults to pointer-to-member connections, which cannot compile
  for forms whose slots live in the derived class; `ananas.pri` sets
  `QMAKE_UIC_FLAGS += -c string` (Qt6 only).
- Ported `libqdataschema` to Qt6 (`qt6` branch): `QString::sprintf`,
  `QTextStream::setEncoding`, `QMetaType` field types, Qt6 debian paths.
- Added `docker/Containerfile.qt6`, `scripts/build-qt6.sh`, `smoke-qt6.sh`,
  `run-qt6.sh`, `build-qt6-qdataschema.sh`.
- `qmake6` drives the active build; a CMake build was added in Phase 5.

## Phase 5 — CMake migration (done) / packaging (deferred)

- CMake migration **done**: `CMakeLists.txt` + per-directory targets
  (`libananas`, `ananasplugin`, `qtscriptedit`, `ananas`,
  `ananas-administrator`, the six extensions, `ananas-test`) with
  AUTOMOC/AUTOUIC/AUTORCC, `AUTOUIC -c string`, `find_library(qdataschema)`,
  the qmake output layout (`lib/`, `lib/designer/`, `bin/`) and `install()`
  rules via `GNUInstallDirs`.
  - Build tree `cmake-build/` (gitignored); qmake files are untouched.
  - Verified: clean configure/build, `ananas-test` 7/7, staged install.
- **Packaging deferred**: `debian/`, `build/*` (RPM/Inno/menus) are kept
  as-is and will be updated for Qt6/CMake later (nothing is deleted). CPack is
  not used; distribution packages will keep using the native tooling
  (debhelper/`rpmbuild`) and call CMake for build+install.

## Inventory scheme packaging (Ubuntu)

The Ubuntu package ships the inventory business scheme so it can be selected at
Ananas startup.

- Schemes: `applications/inventory/inventory.cfg` (main) and
  `inventory-demo.cfg` (demo, differs only in data). Both were authored with
  the Qt3 designer; their embedded forms were converted to Qt4/Qt6 with
  `scripts/port-cfg-ui.py` (Qt4 `uic3` + Qt6 normalization: `QLayoutWidget`,
  `Q3Frame`/`Q3Table` custom widgets, `qPixmapFromMimeSource`, `cstring`).
- The scheme's 28 report templates (`templ_*.odt/.ods/.xml`) come from the Qt3
  tree (`ananas-legacy-qt3/applications/inventory`).
- Demo data: the MySQL dump was converted to a SQLite INSERT-only script with
  `scripts/port-demo-sql.py` (explicit column lists, since the current metadata
  tables have extra columns).
- Packaging: schemes + templates + demo data go to
  `/usr/share/ananas/applications/inventory/`; `inventory.rc` /
  `inventory-demo.rc` to `/etc/ananas/`; a system QSettings registry
  (`/etc/xdg/ananas/ananas.conf`) makes both appear in the startup dialog.
- Runtime (`aDatabase::init`): the database structure is created/updated on
  open; for internal (SQLite) schemes a relative `dbname` is resolved against
  `workdir` (with `~` expanded), the scheme templates are copied into the
  writable `workdir` without overwriting, and the rc `initdata` script is run
  once when the database is first created. Internal schemes skip the login
  dialog.
- Verified: both schemes build a 35-table SQLite DB under `~/.ananas/...`, the
  demo loads its data, templates are provisioned once, and a second run does
  not duplicate data or overwrite templates.

## Phase 6 — Designer (in progress)

- Plan and progress log: `docs/DESIGNER.md`.
- Decision: replace the vendored Qt4 Designer fork (`src/designer/formdesigner/`)
  with a thin wrapper over the **public** Qt6 Designer API
  (`Qt6::Designer` + `libQt6DesignerComponents.so`). Rebasing the fork would
  require vendoring the Qt6 Designer app layer plus its private headers, which
  Ubuntu does not ship; the wrapper needs only public API.
- Part A (wrapper) starts first; Part B ports the metadata editor
  (`src/designer` top level); Part C covers build and packaging.

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

1. `scripts/build-qt6.sh` → `dist/ananas_0.9.6-1_amd64.deb`;
   `scripts/build-qt6-qdataschema.sh` → `dist/libqdataschema_1.0.0-1_amd64.deb`.
2. `scripts/smoke-qt6.sh` (clean container build + `ananas-test` under Xvfb);
   `scripts/port-metrics.sh` for the burndown.
3. Manual GUI smoke: `scripts/run-qt6.sh ananas-administrator`.
4. CMake (in `ananas-qt6-builder`): `cmake -S . -B cmake-build` +
   `cmake --build cmake-build`, then `bin/ananas-test`; staged install via
   `DESTDIR=/tmp/stage cmake --install cmake-build --prefix /usr`.
5. Historical Qt4/Qt5 reproduction: `tools/archive/` (see
   `archive/README.md`).

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
