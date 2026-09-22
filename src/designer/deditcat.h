#ifndef DEDITCAT_H
#define DEDITCAT_H

#include "ui_deditcat.h"

#include <QHash>


class dEditCat : public QMainWindow, public Ui::dEditCat
{
    Q_OBJECT

public:
    dEditCat(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::WindowFlags());
    ~dEditCat();

    aRoleEditor *re;
    QHash<int, int> fields, fieldsg;
    aAliasEditor *al;
    aListViewItem *item;

public slots:
    virtual void setData( aListViewItem * o );
    virtual void updateMD();
    virtual void eSv_activated( int index );
    virtual void eSvG_activated( int index );

protected:
    int newVariable;

protected slots:
    virtual void languageChange();

private:
    void init();
    void destroy();

};

#endif // DEDITCAT_H
