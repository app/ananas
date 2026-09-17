#ifndef DSELECTDB_H
#define DSELECTDB_H

#include "ui_dselectdb.h"
#include <QTreeWidgetItem>


class ANANAS_EXPORT dSelectDB : public QDialog, public Ui::dSelectDB
{
    Q_OBJECT

public:
    dSelectDB(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~dSelectDB();

    QString rcfile;

public slots:
    void newItem();
    void editItem();
    void deleteItem();
    void onCancel();
    void ItemRenamed( QTreeWidgetItem * item, int col );
    void onHelp();
    void createMenu();
    virtual void onDblClick(QTreeWidgetItem *, int);

protected:
    QSettings settings;

    virtual void clearSettings();

protected slots:
    virtual void languageChange();

private:
    int withgroups;
    QMenuBar *menuBar;
    bool changes;
    int ro_mode;
    bool local;

private slots:
    void init();
    void readSettings( QStringList entryGroup );
    void itemSelect();
    void newGroup();
    void saveRC();
    void onOK();
    void importItem();
    void exportItem();

};

#endif // DSELECTDB_H
