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
