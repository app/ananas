# Ananas Designer porting plan (Qt4 → Qt6)

Phase 6 of `PORTING.md`. This document is the working plan and progress log for
porting `ananas-legacy-qt4/src/designer` (the `ananas-designer` application)
to Qt6.

## Scope

`src/designer` has two parts:

1. **Metadata / business-scheme editor** (top-level files: `mainform.*`,
   `cfgform.*`, `dedit*.*`, `actiontree.*`, `interfacetree.*`, `mdtree.*`,
   `roleeditor.*`, `aliaseditor.*`, ~31.6k LOC, 30 `.cpp/.h` + 20 `.ui`).
   Not ported: 326 `Q3*` sites in ~30 files plus Q3 widgets in `.ui`.
2. **`formdesigner/`** (~6.9k LOC): a vendored, almost vanilla fork of the
   Qt4 Designer application (Trolltech 2006). It uses Qt Designer **private**
   headers (`private/pluginmanager_p.h`, `private/qdesigner_formbuilder_p.h`,
   `private/qtundo_p.h`, ...). There is no Ananas-specific logic in it.

Neither part is built today: `src/src.pro` comments the designer out, the CMake
build does not include it, and the `.deb` does not ship it
(`build/ubuntu/rules` is commented out).

## Decision: thin wrapper over the public Qt6 Designer API

Replace the vendored Qt4 Designer fork with a small workbench built on the
**public** Qt6 Designer API (`Qt6::Designer` + `libQt6DesignerComponents.so`).

Rationale (measured against `qttools` v6.4.2, the version shipped by the
Ubuntu 24.04 image):

- The Qt6 Designer **app layer** (`qdesigner_actions.cpp` 52 KB,
  `qdesigner_workbench.cpp` 38 KB, `qdesigner_toolwindow.cpp`,
  `qdesigner_formwindow.cpp`, `qdesigner_settings.cpp`, `appfontdialog.cpp`,
  `newform.cpp`, `saveformastemplate.cpp`, `mainwindow.cpp`, ...) is ~5–6k LOC.
- That app layer includes Qt Designer private headers
  (`QtDesigner/private/pluginmanager_p.h`,
  `QtDesigner/private/formwindowbase_p.h`,
  `QtDesigner/private/actioneditor_p.h`) and references private classes
  (`qdesigner_internal::PreviewManager`, `PreviewConfiguration`,
  `UicLanguage`). Its CMake target links `Qt::CorePrivate`,
  `Qt::DesignerComponentsPrivate`, `Qt::DesignerPrivate`.
- The Ubuntu 24.04 image ships **no private headers** (no `QtCore/6.4.2`, no
  `QtDesigner/private`) and does not install `qt6-base-private-dev`.

So "fork by analogy" / rebase is really a vendoring of the Qt6 Designer app
layer **plus** its private headers **plus** base private dev packages — large,
fragile and version-coupled. The wrapper needs none of that.

The metadata editor only uses a tiny interface from the form designer:

```cpp
class aFormDesigner {
    void fileOpen(const QString&);
    void fileSaveAll();
    void show();
    void hide();
    QDesignerWorkbench *workbench();
};
```

The public Qt6 API covers it:
`QDesignerComponents::createFormEditor()` /
`createWidgetBox()` / `createPropertyEditor()` / `createObjectInspector()` /
`createActionEditor()` / `createResourceEditor()` / `createSignalSlotEditor()`,
`QDesignerFormWindowManagerInterface::createFormWindow()`,
`QDesignerFormWindowInterface::setFileName()/setContents()/contents()`,
`QDesignerIntegration`.

## Working principle: continuous buildability

Every step must end with a green build and green checks before the next step
starts (same rule as `PORTING.md` principle 1). Concretely:

- The main tree (`smoke-qt6.sh`) must keep building and `ananas-test` must stay
  7/7 after every change; the designer lives outside that build, so it must not
  disturb it.
- The new designer target is compiled in the Qt6 image at each step; a step is
  only done when `qmake` + `make` succeed for it.
- Behaviour is verified with a smoke run under Xvfb, not by inspection.
- Intermediate steps are deliberately small: first prove the toolchain/linking
  assumptions with a minimal probe, then grow the workbench.
- The progress log records the exact command run and its result for each step.

## Plan

### Part A — Qt6 form-designer wrapper

1. **New workbench** `src/designer/designer6/aworkbench.{h,cpp}`,
   `aDesignerWorkbench : QMainWindow`:
   - `m_core = QDesignerComponents::createFormEditor(this)`,
     `createTaskMenu(m_core, this)`, `initializePlugins(m_core)`.
   - Dock widgets from `QDesignerComponents::create*` +
     `core->setWidgetBox/setPropertyEditor/setObjectInspector/setActionEditor`.
   - Central `QMdiArea`; forms are `QDesignerFormWindowInterface` (a `QWidget`)
     hosted in `QMdiSubWindow`.
   - `m_integration = new QDesignerIntegration(m_core, this)`.
   - Menus: **File** (Open/Save/Save As/Close/Quit — own actions),
     **Edit/Form/Window** from `core->formWindowManager()->action(...)`.
   - `openForm(path)` (read file → `createFormWindow()` →
     `setFileName()`/`setContents()` → subwindow → `setActiveFormWindow()`),
     `saveForm(fw)` (`contents()` → file), `saveAll()`,
     `formWindowCount()/formWindow(i)`.
2. **Rewrite** `src/designer/formdesigner.{h,cpp}` — `aFormDesigner` over
   `aDesignerWorkbench`, same public API.
3. **Fixture extractor** `tools/scripts/extract-cfg-form.py <cfg> <id|name>
   <out.ui>` — based on `port-cfg-ui.py` (`<dialogform>` + unescape); writes a
   Qt6-loadable `.ui`.
4. **Build**: qmake `src/designer/designer6/designer6.pro`
   (`QT += designer`, `LIBS += -lQt6DesignerComponents`); CMake target
   `designer-smoke` (`Qt6::Designer` + `find_library(Qt6DesignerComponents)`).
   Do **not** use `Qt6::DesignerComponentsPrivate`.
5. **Smoke** `tools/scripts/smoke-designer-qt6.sh`: build + run under Xvfb with
   `QT_PLUGIN_PATH=$REPO/lib/designer`; `designer_smoke.cpp` opens a form,
   checks the form window was created and round-trips `contents()`.
6. **Archive** `src/designer/formdesigner/` → `tools/archive/qt4-designer-fork/`
   and drop the references from `designer.pro`.

### Part B — metadata editor port

7. Port the top-level metadata editor sources Qt3/Qt4 → Qt6 (mechanical, same
   approach as Phase 1/4): `Q3TextStream` → `QTextStream`, Q3 collections →
   Qt6, `QString::null`, `local8Bit()`, `qApp->setMainWidget`, etc.
8. Convert the 20 `.ui` files (uic6, `-c string`).
9. Wire the wrapper into `MainForm`/`dedit*`; remove the old fork references.

### Part C — build & packaging

10. Add `ananas-designer` to qmake + CMake and to the `.deb`; install form
    templates and the widget plugins; update `build/menus/*`.

## Verification

1. Extractor produces a `.ui` from `applications/inventory/inventory.cfg`.
2. `smoke-designer-qt6.sh`: the wrapper opens/saves the form, round-trip OK.
3. Ananas widget classes appear in the widget box (plugin loaded from
   `lib/designer`).
4. After Part B: `ananas-designer` builds and starts; schema tree opens.
5. `port-metrics.sh` shows zero `Q3*`/Qt3Support in `src/designer`.

## Risks

- Custom-widget resolution needs `ananasplugin` on the Designer plugin path.
- Stock Designer features (recent files, backup, preview, code view, appearance
  options, top-level UI mode) are not reproduced; the scheme editor does not
  need them.
- The forms embedded in `.cfg` are escaped Qt3-era XML; the extractor and
  `port-cfg-ui.py` normalization must agree.

## Progress log

- **2026-09-22** — Document created. Decision: Part A (public-API wrapper).
  Effort assessment against `qttools` v6.4.2 done; no code yet.
- **2026-09-22** — **Step 1a (link probe): done.**
  - Added `src/designer/designer6/designer_smoke.cpp` +
    `designer6.pro` (`QT += designer`, `LIBS += -lQt6DesignerComponents`).
  - Image: `libQt6DesignerComponents.prl` pulls `libxkbcommon.so`, whose dev
    symlink was missing; added `libxkbcommon-dev` to
    `docker/Containerfile.qt6` and rebuilt `ananas-qt6-builder`.
  - Gate: `qmake` + `make` OK; `ldd` shows `libQt6DesignerComponents.so.6` /
    `libQt6Designer.so.6`; `designer-smoke` prints
    `designer core ok: qdesigner_internal::FormEditor` and exits 0 under both
    `QT_QPA_PLATFORM=offscreen` and `xvfb-run`.
  - Note: `xvfb-run` can linger on teardown; the smoke wrapper uses `timeout`.
- **2026-09-22** — **Step 1b (fixture extractor): done.**
  - Added `tools/scripts/extract-cfg-form.py <cfg> <id|name> <out.ui>`.
  - Only `applications/inventory/inventory.cfg` form `id=406`
    ("Список документов") carries a `<customwidgets>` block; the other forms
    rely on the plugin widget database.
  - Gate: extracted `tmp/designer-fixtures/inventory-form-406.ui`.
- **2026-09-22** — **Step 1c (open/save round-trip): done.**
  - Added `aworkbench.{h,cpp}`: `createFormEditor` + `createTaskMenu` +
    `initializePlugins` + `QDesignerIntegration`, central `QMdiArea`,
    `openForm`/`saveForm`/`saveAll`/`formWindow*`.
  - `designer_smoke.cpp` now opens a form, saves it and reopens the saved copy.
  - Gate: `make` OK; run under Xvfb with `QT_PLUGIN_PATH=<repo>/lib` and
    `LD_LIBRARY_PATH=<repo>/lib:<repo>/lib/designer` prints
    `OK: opened/saved/reopened ... (2920 bytes)`, exit 0; the Ananas widget
    plugins load (`wDBFieldPlugin::initialize`, `wDBTable init ok`).
  - Known non-fatal warning at this step: `QDesignerIntegration` connects to
    the property editor before it exists
    (`Cannot connect (nullptr)::propertyChanged`); fixed in step 1d when the
    dock components are created and registered on the core.
- **2026-09-22** — **Step 1d (docks/menus) + smoke script: done.**
  - `aworkbench.cpp` now creates the widget box / property editor / object
    inspector / action editor via `QDesignerComponents::create*`, registers
    them on the core and docks them; adds File/Edit/Form/Window menus. The
    property editor exists before `QDesignerIntegration`, so the
    `propertyChanged` warning is gone.
  - Fixed a teardown crash: the dock components call `core->settingsManager()`
    while being destroyed, so `~aDesignerWorkbench` deletes the docks
    explicitly before `QMainWindow` deletes the core (SIGSEGV in
    `QDesignerFormEditorInterface::settingsManager()`).
  - Added `tools/scripts/smoke-designer-qt6.sh` (extracts the fixture on the
    host, builds the main tree + wrapper in the image, runs under Xvfb).
  - Gate: `smoke-designer-qt6.sh` prints
    `OK: opened/saved/reopened ... (2920 bytes)`, exit 0.
- **2026-09-22** — **Step 2 (aFormDesigner facade): done.**
  - `src/designer/formdesigner.{h,cpp}` rewritten: `aFormDesigner` now derives
    from `aDesignerWorkbench` and exposes the legacy `fileOpen`/`fileSaveAll`
    (plus inherited `openForm`/`saveForm`/`saveAll`/`formWindow*`).
  - `designer6.pro` builds `../formdesigner.cpp`; the smoke uses the facade
    (`fileSaveAll` + `fileOpen`).
  - Gate: smoke still prints `OK: opened/saved/reopened ... (2920 bytes)`.
- **2026-09-22** — **Step 3 (archive the Qt4 fork): done.**
  - `src/designer/formdesigner/` (42 files) moved to
    `tools/archive/qt4-designer-fork/`; `archive/README.md` documents it.
  - `src/designer/designer.pro` no longer references the fork; the new
    `designer6/aworkbench.*` is listed instead. (Part B will rework
    `designer.pro` fully.)
- **2026-09-22** — **Step 4 (no regression): done.**
  - `smoke-qt6.sh`: clean build + `ananas-test` **7/7**, `Total fails: 0`.
  - `smoke-designer-qt6.sh`: green after the clean.

### Part B — metadata editor port (done)

- **2026-09-22** — **Part B: done.**
  - `designer.pro` configured for Qt6: dropped `QT += script scripttools`
    and `-lqt4designer`, added `QT += designer qml` and
    `-lQt6DesignerComponents`; `src/designer/formdesigner/` references
    already removed (Part A).
  - Added two porting helpers: `scripts/port-designer-ui.py` (Qt4 `.ui`
    with Qt3 widgets -> Qt6 `.ui`) and `scripts/port-designer-q3.py`
    (mechanical Qt3 class/QString renames in C++).
  - Converted all 19 `.ui` files; they pass `uic -c string` on Qt6.
  - Ported the C++ sources: `QWorkspace`->`QMdiArea`, `Q3ListView`->
    `QTreeWidget`, `Q3Table`->`QTableWidget`, `Q3IntDict`->`QHash`,
    `Q3TextEdit`/`ScriptEdit`->`QPlainTextEdit` accessors, `Q3PopupMenu`->
    `QMenu`, `Q3FileDialog`->`QFileDialog`, `QString::sprintf`->`arg`,
    `QWorkspace`/`setCaption`/`QMAX`/`QDir::homeDirPath` and the removed
    `QMessageBox::warning(..., button texts)` overload, `QSplashScreen::message`
    ->`showMessage`, etc.
  - Dead Qt3 Trolltech code `embed.{cpp,h}` moved to
    `tools/archive/qt4-designer-fork/`.
  - `port-metrics.sh` now includes `src/designer`; **Q3* / Qt3Support / q3
    includes / Q3 `.ui` are all 0**.
  - Gates: `ananas-designer` builds and links; `smoke-designer-qt6.sh`
    green (wrapper round-trip + `ananas-designer --help` starts MainForm);
    `smoke-qt6.sh` still **7/7**.
  - Known follow-ups: some string-based `connect()`s still use Qt3 signal
    signatures and are runtime no-ops (tab change, item rename/double-click);
    item rename via `editItem()` needs `Qt::ItemIsEditable`. These are
    behavioral, not build, issues and are tracked for Part C/manual smoke.

### Part C — build & packaging (done)

- **2026-09-22** — **Part C: done.**
  - `src/src.pro` builds `designer` as part of the main tree, so `make` and
    `smoke-qt6.sh` now compile `ananas-designer` too.
  - `designer.pro` gets an RPATH to `/usr/lib/ananas/designer` (widget plugin).
  - `build/ubuntu/rules` installs `ananas-designer.desktop`; the `.deb`
    already picks up `/usr/bin/ananas-designer`, `/usr/lib/libqtscriptedit.so*`,
    the designer translations and icon via the subdir `install` rules.
  - Runtime fixes found by the GUI smoke: `QAction::activated()` ->
    `triggered()` (in `.ui` `<connections>` and `.cpp`), and the Qt3
    `Q3Table`/`Q3IconView`/`QTabWidget` signals in `cfgform.ui` ->
    `cellDoubleClicked(int,int)`, `itemChanged(QListWidgetItem*)`,
    `currentChanged(int)` with matching slot signatures.
  - Gates: `ananas-designer` builds; `smoke-designer-qt6.sh` green; the
    packaged app starts on the inventory scheme (`--rc=/etc/ananas/inventory.rc`
    under Xvfb), creates the SQLite DB and runs without connect warnings;
    `smoke-qt6.sh` **7/7**; `port-metrics.sh` Q3 = 0.
  - Note: `dh_shlibdeps` resolves `libqt6designer6` /
    `libqt6designercomponents6` from their shlibs, so no manual `Depends`
    entry is needed.

### Runtime smoke fixes

- **2026-09-22** — **First runtime errors from the GUI smoke: fixed.**
  - `dEditDialog::formPreview()` built the form with a bare `QFormBuilder`
    that had no plugin path, so custom widgets (e.g. `wCatalogue`) failed with
    `QFormBuilder was unable to create a widget of the class 'wCatalogue'`.
    Added the same plugin paths as `aForm`: `applicationDirPath()`,
    `<appdir>/../lib/designer` and `/usr/lib/ananas/designer`. Verified with a
    standalone `QFormBuilder` probe: the catalogue form fails without the
    plugin path and loads with it.
  - Stripped the stale `iCCP` chunk from the designer splash PNGs (libpng
    `known incorrect sRGB profile` warning); added `strip-png-iccp.py`.
  - Gate: package builds; the installed app starts with no libpng warning.
- **2026-09-22** — **Widget-box drag-and-drop fixed.**
  - The widget box starts a drag via
    `core->formWindowManager()->dragItems()`, which passes `core->topLevel()`
    to `QDesignerMimeData::execDrag()` as the drag source. The workbench never
    called `core->setTopLevel()`, so `QDrag` had no source and dropping a
    widget on a form did nothing.
  - `aDesignerWorkbench` now calls `m_core->setTopLevel(this)` and, after
    opening a form, `formWindow->editWidgets()` (activate the widget tool);
    the smoke asserts `core->topLevel()`.
  - Note: Qt6 Designer's widget box only supports drag-and-drop, not
    double-click insertion (the list view only handles `pressed`).
- **2026-09-22** — **Widget property editors did not open.**
  - `aWidget::widgetEditor()` created the editor via `createEditor()` and only
    then checked that the widget's top-level window was named
    `ananas-designer_mainwindow`. In the form designer the edited form lives in
    the separate designer window, so the check failed and the dialog was
    created and discarded without being shown (the leaked dialog produced the
    `QObject::connect` warnings for its UI).
  - The editor now calls `aWidget::getMd()`, which locates the designer main
    window among all top-level widgets (and returns null in the runtime
    engine); the dialog is deleted when no metadata is available.
  - Also fixed the Qt3 signals in the plugin forms: `QComboBox
    activated(QString)` -> `textActivated(QString)` and `QListWidget
    highlighted(int)` -> `currentRowChanged(int)` (`efield.ui`,
    `edbfield.ui`, `edbtable.ui`, `etable.ui`).
- **2026-09-22** — **Crash opening the wDBTable editor.**
  - `eDBTable::setData()` populated `ListCol` before assigning the
    `cwidth`/`fname`/`idlist` lists. Inserting the first header emits
    `currentRowChanged`, which calls `ColumnSel()` and indexes those lists
    (still empty), asserting on the Qt6 `QStringList` bounds check.
  - Assign the lists before inserting the headers and guard the indexing in
    `ColumnSel()`, `getData()` and `ColWidthChange()`.
  - The form editor wrote its temporary `inputform_<id>.ui` files into the
    process working directory; they now go to `QDir::tempPath()` (and the
    preview file is removed after loading).
- **2026-09-22** — **Crash opening the wDBField editor.**
  - `wDBField::getFields()` called `md->attr(..., mda_type).at(0)` on a
    possibly empty type attribute; Qt6 asserts on an out-of-range
    `QString::at()` (Qt3 returned a null QChar). This aborted the process
    from `wDBFieldTaskMenu::edit()` → `aWidget::widgetEditor()` →
    `addfdialog::setData()`.
  - Guard the empty string in both loops, and apply the same fix to the
    identical `type.section(...).at(0)` pattern in `aField`/`aDataField`
    constructors and to `aService::parts2money()`.
  - Also fixed `aCfg::init()`: it declared a local `rootnode`, shadowing the
    member, so `find(mdc_root)` returned an empty configuration instead of
    the loaded one.
  - Found with a temporary SIGABRT/SIGSEGV backtrace handler and trace prints
    (removed after the fix); `build-qt6-worktree.sh` was added to build a
    `.deb` from the working tree for such debugging.
