// Smoke test for the Qt6 form-designer wrapper (Phase 6, steps 1a-2).
//
// Opens a form, saves it and reopens the saved copy to prove the round-trip,
// exercising the aFormDesigner facade used by the metadata editor. Run under
// Xvfb (or QT_QPA_PLATFORM=offscreen) with the Ananas widget plugin on the
// Designer plugin path.

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <QtDesigner/QDesignerComponents>
#include <QtDesigner/abstractformeditor.h>
#include <QtDesigner/abstractformwindow.h>

#include "formdesigner.h"

static int fail(const QString &message)
{
    qCritical().noquote() << "FAIL:" << message;
    return 1;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if (argc < 2)
        return fail("usage: designer-smoke <form.ui>");

    QDesignerComponents::initializeResources();

    const QString source = QString::fromLocal8Bit(argv[1]);
    const QString roundtrip = QDir::tempPath() + "/designer-smoke-roundtrip.ui";
    QFile::remove(roundtrip);
    if (!QFile::copy(source, roundtrip))
        return fail("cannot copy " + source + " to " + roundtrip);

    aFormDesigner designer;
    QString error;

    if (!designer.openForm(roundtrip, &error))
        return fail("openForm: " + error);

    if (designer.formWindowCount() != 1)
        return fail(QString("expected 1 form window, got %1").arg(designer.formWindowCount()));

    if (!designer.core()->topLevel())
        return fail("core->topLevel() is null: widget box drag-and-drop will not work");

    designer.fileSaveAll();
    if (QFileInfo(roundtrip).size() == 0)
        return fail("saved file is empty");

    // Reopen the saved form through the facade to prove the output is
    // loadable. (Only one form editor may exist per process.)
    designer.fileOpen(roundtrip);
    if (designer.formWindowCount() != 2)
        return fail(QString("expected 2 form windows, got %1").arg(designer.formWindowCount()));

    qInfo().noquote() << "OK: opened/saved/reopened" << source
                      << "(" << QFileInfo(roundtrip).size() << "bytes )";
    return 0;
}
