#ifndef AWORKBENCH_H
#define AWORKBENCH_H

#include <QList>
#include <QMainWindow>

class QDockWidget;
class QMdiArea;
class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;

// Minimal Qt6 form-designer workbench built on the public Designer API.
// It replaces the vendored Qt4 Designer fork (Phase 6, part A).
class aDesignerWorkbench : public QMainWindow
{
    Q_OBJECT

public:
    explicit aDesignerWorkbench(QWidget *parent = nullptr);
    ~aDesignerWorkbench() override;

    QDesignerFormEditorInterface *core() const;

    QDesignerFormWindowInterface *openForm(const QString &fileName,
                                           QString *errorMessage = nullptr);
    bool saveForm(QDesignerFormWindowInterface *formWindow);
    int saveAll();

    int formWindowCount() const;
    QDesignerFormWindowInterface *formWindow(int index) const;
    QDesignerFormWindowInterface *activeFormWindow() const;

public slots:
    void openFormDialog();

private:
    void createMenus();

    QDesignerFormEditorInterface *m_core = nullptr;
    QMdiArea *m_mdiArea = nullptr;
    QList<QDockWidget *> m_docks;
};

#endif // AWORKBENCH_H
