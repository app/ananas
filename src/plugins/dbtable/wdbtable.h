/****************************************************************************
** $Id: wdbtable.h,v 1.1 2008/11/05 21:16:30 leader Exp $
**
** Header file of the database table plugin of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
** Copyright (C) 2003-2004 Grigory Panov, Yoshkar-Ola.
**
** This file is part of the Ananas Plugins of the Ananas
** automation accounting system.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.leaderit.ru/page=ananas or email sales@leaderit.ru
** See http://www.leaderit.ru/gpl/ for GPL licensing information.
**
** Contact org@leaderit.ru if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef WDBTABLE_H
#define WDBTABLE_H

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStyledItemDelegate>
#include <QSqlRecord>
#include <QSqlIndex>
#include <QSqlError>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <QFrame>
#include <QList>
#include <QPixmap>
#include <QKeyEvent>
#include <QEvent>
#include "acfg.h"
#include "aobject.h"
#include "awidget.h"
#include "wfield.h"

class aDatabase;
class wDBTable;
class aDataTable;

/* operation kind for line updates (was QSql::Op, removed in Qt5/6) */
enum aTableOp { aOpNone = -1, aOpInsert = 0, aOpUpdate = 1, aOpDelete = 2 };

class aSearchWidget : public QFrame
{
Q_OBJECT
public:
	aSearchWidget( QWidget *parent, wDBTable *table );
	~aSearchWidget();
	void search( const QString &t ="");
public slots:
	void setText( const QString &t );
protected:
        bool eventFilter( QObject *obj, QEvent *ev );
private:
	QString ftext;
	QLineEdit *l;
	wDBTable *t;
};



/*!
 * \en	Class for editing documents tables. \_en
 * \ru
 * 	\brief Визуальный класс для редактирования табличных частей документа.
 * 	Наследует QTableWidget.
 *
 * 	Поддерживает редактирование журнала документов
 * 	и таблицы элементов каталога.
 * \_ru
 *
 */
class QT_WIDGET_PLUGIN_EXPORT wDBTable : public QTableWidget
{
	friend class aForm;

	Q_OBJECT
	Q_PROPERTY( qulonglong	Id READ getId WRITE setId DESIGNABLE true )
	Q_PROPERTY( int		DefaultColWidth READ getDefColWidth WRITE setDefColWidth DESIGNABLE true )
	Q_PROPERTY( int		TableInd READ getTblInd WRITE setTblInd DESIGNABLE true )
	Q_PROPERTY( QStringList	DefFields READ getDefFields WRITE setDefFields DESIGNABLE true )
	Q_PROPERTY( QStringList	DefHeaders READ getDefHeaders WRITE setDefHeaders DESIGNABLE true )
	Q_PROPERTY( QStringList	ColWidth READ getColWidth WRITE setColWidth DESIGNABLE true )
	Q_PROPERTY( QStringList	DefIdList READ getDefIdList WRITE setDefIdList DESIGNABLE true )
	Q_PROPERTY( QString	editFormName READ getEditFormName WRITE setEditFormName DESIGNABLE true )
public:
	enum RefreshMode { RefreshAll = 0, RefreshColumns = 1 };

	aCfg*		md;
	aDatabase*	db;
	aEngine*	engine;
	QStringList	list_available_tables;
	QStringList	fname, hname, colWidth,idList;
	int		defColWidth;
	int		tableInd;
	aCfgItem	tables;
	bool		searchMode;
	QString		searchString;
	aSearchWidget	*searchWidget;

	wDBTable( QString objtype, QWidget *parent = 0, const char *name = 0);
	virtual ~wDBTable();
	void		checkFields();
	void		setFields(int numTable); //set propertis default value for table with num=numTable
	QStringList	getFields(int numTable, bool GetId=false); // return list field names or id's, defined in metadata
	QString		getFieldName(long idField); // return attribute `name' value
	QString		getFieldType(long idField); // return attribute `type' value
	int		getTableInd(int id);
	int		getTableId(int ind);

	void		init();
	void		init(aDatabase *adb, aEngine *e = 0 ); // call in ananas engine
	void		OpenEditor();

	QStringList	getDefFields()	const;
	QStringList	getDefHeaders()	const;
	QStringList	getColWidth() 	const;
	QStringList	getDefIdList()	const;
	qulonglong	getId();
	int 		getTblInd() 	const;
	bool		getOpenEditor() const;
	int		getDefColWidth() const;

	void 	setDefFields(QStringList lst);
	void 	setDefHeaders(QStringList lst);
	void 	setColWidth(QStringList lst);
	void 	setDefIdList(QStringList lst);
	void 	setId( qulonglong fn );
	void	setTblInd( int ind );
	void 	setOpenEditor( bool fn );
	void 	setDefColWidth( int fn );

	QString	getEditFormName() const	{ return vEditFormName;};
	void 	setEditFormName( QString name)	{ vEditFormName = name;	};

	QString	containerType()	const { return container_type;};
	void 	setContainerType( QString name)	{ container_type = name; };
	QString	columnField( int col ) const { return m_columns.value( col ); }
	QPixmap systemIcon();
	virtual int Select( ANANAS_UID db_uid );
	aCfg*	getMd();

	/* compatibility helpers replacing the legacy table API */
	aDataTable*	sqlCursor() const { return m_table; }
	void		setSqlCursor( aDataTable *t ) { m_table = t; }
	int		numCols() const { return columnCount(); }
	int		numRows() const { return rowCount(); }
	void		addColumn( const QString &field, const QString &header, int width = 100 );
	void		removeColumn( int col );
	void		setFilter( const QString &flt );
	void		setColumnReadOnly( int col, bool ro );
	void		setReadOnly( bool ro );
	bool		isReadOnly() const;
	QSqlRecord*	currentRecord();
	void		refresh( int mode = RefreshAll );
	QVariant	value( int row, int col );

public slots:
	void	refreshAll();
	QList<int> getBindList();
	void 	setWFieldEditor();
	void	setAvailableTables();
	void 	lineUpdate(aTableOp mode);
	void	newFilter(const QString & );
	void	newDataId(const qulonglong );
	QVariant Value( const QString &colname );

	bool searchColumn( const QString &text, bool FromCurrent = false, bool Forward = true );
	void searchOpen( const QString &text = "" );
	void searchClose();

protected slots:
	void doubleClickEventHandler(int , int ); //parametrs not used
	virtual void updateTableCellHandler(int, int);
signals:

/*!
 *	\~english
 *	Signal emitted after table line update.
 *	\~russian
 *	Сигнал испускается после обновлении строки таблицы.
 *	\~
 */
	void saveLine(QSqlRecord *rec);

/*!
 *	\~english
 *	Signal emitted after table line delete.
 *	\~russian
 *	Сигнал испускается после удаления строки таблицы.
 *	\~
 */
	void deleteLine(QSqlRecord *rec);
/*!
 *	\~english
 *	Signal emitted after select another document.
 *	\~russian
 *	Сигнал испускается после выбора документа.
 *	\~
 */
	void selected( qulonglong uid );
/*!
 *	\~english
 *	Signal emitted after change line.
 *	\~russian
 *	Сигнал испускается после изменения строки.
 *	\~
 */
	void selectRecord ( qulonglong );
/*!
 *	\~english
 *	Signal emitted after update cell.
 *	\~russian
 *	Сигнал испускается после обновления ячейки с номером \a row, \a col.
 *	\~
 */
	void updateCurr(int row, int col);
	void currentChanged( const QSqlRecord *record );

	//signals from context menu for connecting to wJournal
	void insertRequest();
	void updateRequest();
	void deleteRequest();
	void viewRequest();
	// end
	void getMd( aCfg ** );
	void getId( qulonglong * );

private slots:
	void lineChange(int, int);
	void lineInsert(QSqlRecord*);
	void updateItem( ANANAS_UID db_uid );

protected:
	virtual bool updateCurrent();
	virtual void contextMenuEvent ( QContextMenuEvent * e );
	virtual bool deleteCurrent();
	virtual void keyPressEvent ( QKeyEvent *e );
	void EditElement();
	virtual bool beginInsert ();
	virtual bool confirmEdit( aTableOp m );
	QString	displayValue( const QString & field ) const;

private:
	QStringList	m_columns;	// db field name per column
	aDataTable	*m_table;
	bool		m_populating;

	QPixmap t_doc;
	QPixmap t_doc_d;
	QPixmap t_doc_t;
	QPixmap t_doc_m;
	QPixmap t_doc_tm;
	QPixmap t_cat_e;
	QPixmap t_cat_ed;
	QPixmap t_cat_g;
	QPixmap t_cat_gd;

	int lastEditedRow;
	int lastEditedCol;

	QString container_type;
	qulonglong	doc_id;
	qulonglong	cat_group_id;
	long journalFieldId(long);
	QString journalFieldName(long);
	bool inEditMode;
	QList<int> listBindings;
	aCfgItem obj;
        QString vName, vEditFormName;
	QString	vDefineCols;
	qulonglong oid;
};


/*!
 * \en	Class for support custom editor in wDBTable. \_en
 * \ru
 * 	\brief Класс для поддержки собственного редактора поля в wDBTable.
 * 	Наследует QStyledItemDelegate.
 * \_ru
 */
class aEditorFactory: public QStyledItemDelegate
{
public:
	aEditorFactory( wDBTable *table );
	QWidget * createEditor (QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void setMd(aCfg *md);
private:
	wDBTable *m_table;
	aCfg * md;
};
#endif
