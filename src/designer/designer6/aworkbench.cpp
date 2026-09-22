#include "aworkbench.h"

#include <QAction>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMdiArea>
#include <QMdiSubWindow>

#include <QtDesigner/QDesignerComponents>
#include <QtDesigner/QDesignerIntegration>
#include <QtDesigner/abstractactioneditor.h>
#include <QtDesigner/abstractformeditor.h>
#include <QtDesigner/abstractformwindow.h>
#include <QtDesigner/abstractformwindowmanager.h>
#include <QtDesigner/abstractobjectinspector.h>
#include <QtDesigner/abstractpropertyeditor.h>
#include <QtDesigner/abstractwidgetbox.h>

aDesignerWorkbench::aDesignerWorkbench(QWidget *parent)
    : QMainWindow(parent)
    , m_core(QDesignerComponents::createFormEditor(this))
{
    setWindowTitle(tr("Dialog forms designer"));

    m_mdiArea = new QMdiArea(this);
    setCentralWidget(m_mdiArea);

    QDesignerComponents::createTaskMenu(m_core, this);
    QDesignerComponents::initializePlugins(m_core);

    // Register the standard Designer tool windows on the core. The property
    // editor must exist before the integration is created, otherwise the
    // integration cannot connect to its propertyChanged() signal.
    struct DockSpec {
        const char *objectName;
        const char *title;
        Qt::DockWidgetArea area;
        QWidget *widget;
    };

    QDesignerWidgetBoxInterface *widgetBox = QDesignerComponents::createWidgetBox(m_core, this);
    QDesignerPropertyEditorInterface *propertyEditor = QDesignerComponents::createPropertyEditor(m_core, this);
    QDesignerObjectInspectorInterface *objectInspector = QDesignerComponents::createObjectInspector(m_core, this);
    QDesignerActionEditorInterface *actionEditor = QDesignerComponents::createActionEditor(m_core, this);

    m_core->setWidgetBox(widgetBox);
    m_core->setPropertyEditor(propertyEditor);
    m_core->setObjectInspector(objectInspector);
    m_core->setActionEditor(actionEditor);

    const DockSpec docks[] = {
        { "widgetBoxDock",      QT_TRANSLATE_NOOP("aDesignerWorkbench", "Widget Box"),       Qt::LeftDockWidgetArea,  widgetBox },
        { "objectInspectorDock", QT_TRANSLATE_NOOP("aDesignerWorkbench", "Object Inspector"), Qt::RightDockWidgetArea, objectInspector },
        { "propertyEditorDock", QT_TRANSLATE_NOOP("aDesignerWorkbench", "Property Editor"),  Qt::RightDockWidgetArea, propertyEditor },
        { "actionEditorDock",   QT_TRANSLATE_NOOP("aDesignerWorkbench", "Action Editor"),    Qt::BottomDockWidgetArea, actionEditor },
    };

    for (const DockSpec &spec : docks) {
        QDockWidget *dock = new QDockWidget(tr(spec.title), this);
        dock->setObjectName(QLatin1String(spec.objectName));
        dock->setWidget(spec.widget);
        addDockWidget(spec.area, dock);
        m_docks.append(dock);
    }

    new QDesignerIntegration(m_core, this);

    // The widget box drag uses core->topLevel() as the drag source; without it
    // dragging widgets onto a form does nothing.
    m_core->setTopLevel(this);

    createMenus();
}

aDesignerWorkbench::~aDesignerWorkbench()
{
    // The DesignerComponents widgets access the core while being destroyed
    // (e.g. via settingsManager()), so they must go before the core, which
    // QMainWindow deletes with the rest of its children.
    qDeleteAll(m_docks);
    m_docks.clear();
}

QDesignerFormEditorInterface *aDesignerWorkbench::core() const
{
    return m_core;
}

void aDesignerWorkbench::createMenus()
{
    QDesignerFormWindowManagerInterface *manager = m_core->formWindowManager();

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openAction = fileMenu->addAction(tr("&Open..."), this, &aDesignerWorkbench::openFormDialog);
    openAction->setShortcut(QKeySequence::Open);

    QAction *saveAction = fileMenu->addAction(tr("&Save"), this, [this] {
        saveForm(activeFormWindow());
    });
    saveAction->setShortcut(QKeySequence::Save);

    fileMenu->addAction(tr("Save &All"), this, [this] { saveAll(); });
    fileMenu->addSeparator();
    QAction *closeAction = fileMenu->addAction(tr("&Close"), this, &QWidget::close);
    closeAction->setShortcut(QKeySequence::Close);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    const QDesignerFormWindowManagerInterface::Action editActions[] = {
        QDesignerFormWindowManagerInterface::UndoAction,
        QDesignerFormWindowManagerInterface::RedoAction,
    };
    for (auto action : editActions)
        editMenu->addAction(manager->action(action));
    editMenu->addSeparator();
#if QT_CONFIG(clipboard)
    const QDesignerFormWindowManagerInterface::Action clipboardActions[] = {
        QDesignerFormWindowManagerInterface::CutAction,
        QDesignerFormWindowManagerInterface::CopyAction,
        QDesignerFormWindowManagerInterface::PasteAction,
    };
    for (auto action : clipboardActions)
        editMenu->addAction(manager->action(action));
    editMenu->addSeparator();
#endif
    editMenu->addAction(manager->action(QDesignerFormWindowManagerInterface::DeleteAction));
    editMenu->addAction(manager->action(QDesignerFormWindowManagerInterface::SelectAllAction));

    QMenu *formMenu = menuBar()->addMenu(tr("F&orm"));
    const QDesignerFormWindowManagerInterface::Action layoutActions[] = {
        QDesignerFormWindowManagerInterface::HorizontalLayoutAction,
        QDesignerFormWindowManagerInterface::VerticalLayoutAction,
        QDesignerFormWindowManagerInterface::GridLayoutAction,
        QDesignerFormWindowManagerInterface::FormLayoutAction,
        QDesignerFormWindowManagerInterface::BreakLayoutAction,
        QDesignerFormWindowManagerInterface::AdjustSizeAction,
    };
    for (auto action : layoutActions)
        formMenu->addAction(manager->action(action));
    formMenu->addSeparator();
    formMenu->addAction(manager->action(QDesignerFormWindowManagerInterface::DefaultPreviewAction));

    menuBar()->addMenu(tr("&Window"));
}

QDesignerFormWindowInterface *aDesignerWorkbench::openForm(const QString &fileName,
                                                           QString *errorMessage)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        if (errorMessage)
            *errorMessage = file.errorString();
        return nullptr;
    }

    QDesignerFormWindowManagerInterface *manager = m_core->formWindowManager();
    QDesignerFormWindowInterface *formWindow = manager->createFormWindow();
    formWindow->setFileName(fileName);

    if (!formWindow->setContents(&file, errorMessage)) {
        manager->removeFormWindow(formWindow);
        formWindow->deleteLater();
        return nullptr;
    }

    m_mdiArea->addSubWindow(formWindow);
    formWindow->show();
    manager->setActiveFormWindow(formWindow);

    // Activate the widget-editing tool so the widget box can insert widgets.
    formWindow->editWidgets();

    return formWindow;
}

void aDesignerWorkbench::openFormDialog()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open Form"), QString(),
                                                          tr("Designer UI files (*.ui);;All files (*)"));
    if (!fileName.isEmpty())
        openForm(fileName);
}

bool aDesignerWorkbench::saveForm(QDesignerFormWindowInterface *formWindow)
{
    if (!formWindow || formWindow->fileName().isEmpty())
        return false;

    QFile file(formWindow->fileName());
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return false;

    return file.write(formWindow->contents().toUtf8()) >= 0;
}

int aDesignerWorkbench::saveAll()
{
    QDesignerFormWindowManagerInterface *manager = m_core->formWindowManager();
    int saved = 0;
    for (int i = 0; i < manager->formWindowCount(); ++i) {
        if (saveForm(manager->formWindow(i)))
            ++saved;
    }
    return saved;
}

int aDesignerWorkbench::formWindowCount() const
{
    return m_core->formWindowManager()->formWindowCount();
}

QDesignerFormWindowInterface *aDesignerWorkbench::formWindow(int index) const
{
    return m_core->formWindowManager()->formWindow(index);
}

QDesignerFormWindowInterface *aDesignerWorkbench::activeFormWindow() const
{
    return m_core->formWindowManager()->activeFormWindow();
}
