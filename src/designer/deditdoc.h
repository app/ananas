#ifndef DEDITDOC_H
#define DEDITDOC_H

#include <QHash>
#include "ui_deditdoc.h"


class dEditDoc : public QMainWindow, public Ui::dEditDoc
{
    Q_OBJECT

public:
    dEditDoc(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::WindowFlags());
    ~dEditDoc();

    aListViewItem *item;
    aAliasEditor *al;
    QHash<int,int> fields;
    aRoleEditor *re;

public slots:
    virtual void setData( aListViewItem * o );
    virtual void updateMD();
    virtual void eSv_activated( int index );

protected slots:
    virtual void languageChange();

    virtual void init();
    virtual void destroy();


};

#endif // DEDITDOC_H
