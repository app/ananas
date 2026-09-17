#ifndef CATALOGFORM_H
#define CATALOGFORM_H

#include "ui_catalogform.h"

#include <QGridLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>


class CatalogForm : public QMainWindow, public Ui::CatalogForm
{
    Q_OBJECT

public:
    CatalogForm(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::WType_TopLevel);
    ~CatalogForm();

    aListView* ListView;
    QString fieldName;
    aLineEdit *LineEdit;
    aListBox* ListHint;
    QFrame* StatusFrame;
    aCatalogue* cat;
    bool FormToSelect;

    virtual Q_ULLONG getId();
    virtual void init();
    virtual void destroy();
    virtual long findFirst( const QString & s );
    virtual Q_ULLONG getIdg( Q_ULLONG ide );
    virtual QPixmap getGroupPixmap();
    virtual void edit( QTreeWidgetItem * item, bool );
    virtual QPixmap getElementPixmap();
    virtual QPixmap getMarkDeletedPixmap();

public slots:
    virtual void setData( aCatalogue * catalog, QMap<Q_ULLONG, QTreeWidgetItem *> mg, const QStringList & Fname, const QStringList & FnameGroup, Q_ULLONG idElForm, Q_ULLONG idGrForm, const bool toSelect );
    virtual void setId( Q_ULLONG idx );
    virtual void find( const QString & s );
    virtual void pressArrow();
    virtual void isPressArrow();
    virtual void setText( const QString & s );
    virtual void go();
    virtual void goToItem( QTreeWidgetItem * item );
    virtual void loadElements( Q_ULLONG idGroup );
    virtual void onLoadElements( QTreeWidgetItem * item );
    virtual void new_item( QTreeWidgetItem * parentItem );
    virtual void new_group( QTreeWidgetItem * parentItem );
    virtual void del_item( QTreeWidgetItem * item );
    virtual void mark_deleted( QTreeWidgetItem * item );
    virtual void undo_mark_deleted( QTreeWidgetItem * item );
    virtual void edit( QTreeWidgetItem * item, int );
    virtual void select( QTreeWidgetItem * item );
    virtual void Refresh( Q_ULLONG id );

signals:
    void selected(Q_ULLONG);

protected:
    QGridLayout* GridLayout;
    Q_ULLONG id,idElementForm,idGroupForm;
    QStringList fieldList, fieldListGroup;

    virtual long getGroupId( QTreeWidgetItem * item );
    virtual Q_ULLONG getElementId( QTreeWidgetItem * item );

protected slots:
    virtual void languageChange();

    virtual void keyPressEvent( QKeyEvent * e );


private:
    QMap<Q_ULLONG, QTreeWidgetItem*> map_gr;
    QMap<Q_ULLONG, QTreeWidgetItem*> map_el;
    QMap<Q_ULLONG,bool> map_deleted;

private slots:
    virtual void doOk();

};

#endif // CATALOGFORM_H
