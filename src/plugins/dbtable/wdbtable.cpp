/****************************************************************************
** $Id: wdbtable.cpp,v 1.1 2008/11/05 21:16:30 leader Exp $
**
** Code file of the database table plugin of Ananas
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

#include <stdlib.h>
#include <qpainter.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QList>
#include <QPixmap>
#include <QFrame>
#include <QMenu>
#include <QEvent>
#include <QPointer>
#include <QHeaderView>
#include <QStyleOptionViewItem>
#include <QModelIndex>

#include "adocjournal.h"
#include "alog.h"
#include "asqltable.h"

static QObjectList aQueryList( QObject *parent, const char *type )
{
	QObjectList res;
	if ( !parent || !type ) return res;
	QObjectList all = parent->findChildren<QObject*>();
	for ( int i = 0; i < all.size(); ++i )
		if ( all[i]->inherits( type ) )
			res << all[i];
	return res;
}

#include "wdbtable.h"
#include "edbtable.h"


/*!
 * \en	Constructor. \_en
 * \ru	Конструктор.
 * 	Задает значение ширины по умолчанию для столбца = 100. \_ru
 */
wDBTable::wDBTable( QString objtype,  QWidget *parent, const char *name )
    : QTableWidget( parent )
{
	setObjectName( name );
	m_table = 0;
	m_populating = false;
	objtype = "";
	defColWidth = 100; //default column width
	tableInd = -1;
	doc_id = 0;
	inEditMode = false;
	searchWidget = 0;
	searchMode = false;
	searchString = "";
	connect( this, SIGNAL(currentCellChanged ( int, int, int, int ) ), this, SLOT(lineChange( int, int ) ) );
	connect( this, SIGNAL(cellChanged ( int, int ) ), this, SLOT(updateTableCellHandler(int, int ) ) );
	connect( this, SIGNAL(cellDoubleClicked ( int, int ) ), this, SLOT(doubleClickEventHandler(int, int ) ) );
	init();


	aLog::print(aLog::Debug, tr("wDBTable init ok"));

}



/*!
 * \en	Opens property editor. \_en
 * \ru	Открывает окно редактора свойсв, если fn == true \_ru
 */
void
wDBTable::setOpenEditor( bool fn )
{
	if(fn)	OpenEditor();
}



/*!
 *	Gets list of defined fields.
 */
QStringList
wDBTable::getDefFields() const
{
	return fname;
}



/*!
 *	Gets list of defined headers.
 */
QStringList
wDBTable::getDefHeaders() const
{
	return hname;
}



/*!
 *	Gets list of defined column width.
 */
QStringList
wDBTable::getColWidth() const
{
	return colWidth;
}



/*!
 *	Gets list of defined column id.
 */
QStringList
wDBTable::getDefIdList() const
{
	return idList;
}

/*!
 *	Gets metadata document id.
 */
qulonglong
wDBTable::getId()
{
	if (doc_id == 0)
	{
		foreach (QWidget *widget, QApplication::topLevelWidgets())
		{
			if (widget->objectName() == QString("ananas-designer_mainwindow") )
			{
				connect( this, SIGNAL( getId( qulonglong * ) ), widget, SLOT( getId( qulonglong * ) ));
				emit ( getId( &doc_id ) );
			}
		}
	}
	return doc_id;
}

/*!
 *	Gets current table id.
 */
int
wDBTable::getTblInd() const
{
	return tableInd;
}

/*!
 *	Always return false.
 */
bool
wDBTable::getOpenEditor() const
{
	return false;
}

/*!
 *	Gets default column width.
 */
int
wDBTable::getDefColWidth() const
{
	return defColWidth;
}

/*!
 *	Sets list of field names.
 */
void
wDBTable::setDefFields(QStringList lst)
{
	fname = lst;
}

/*!
 *	Sets list of field headers.
 */
void
wDBTable::setDefHeaders(QStringList lst)
{
	hname = lst;
}

/*!
 *	Sets list of columns width.
 */
void
wDBTable::setColWidth(QStringList lst)
{
	colWidth = lst;
}

/*!
 *	Gets list of column id.
 */
void
wDBTable::setDefIdList(QStringList lst)
{
	idList = lst;
}

/*!
 *	Sets metadata document id.
 */
void
wDBTable::setId( qulonglong fn )
{
	doc_id = fn;
}


/*!
 *	Gets current table id.
 */
void
wDBTable::setTblInd( int ind )
{
	tableInd = ind;
}


/*!
 *	Sets default columns width.
 */
void
wDBTable::setDefColWidth( int fn )
{
	defColWidth = fn;
}



/*!
 * \en	Creates property editor dialog window. \_en
 * \ru	Создает диалоговое окно редактора свойств. \_ru
 */
void
wDBTable::OpenEditor()
{
	setAvailableTables();
	eDBTable e( this->topLevelWidget());
	checkFields();
	e.setData(this,md);
	if ( e.exec()==QDialog::Accepted )
	{
		e.getData(this);
	}
}



/*!
 *	Destructor.
 */
wDBTable::~wDBTable()
{
	delete m_table;
	m_table = 0;
}

/*!
 *	Return toplevel metadata configuration.
 */
aCfg*
wDBTable::getMd()
{
	foreach (QWidget *widget, QApplication::topLevelWidgets())
	{
		if (widget->objectName() == QString("ananas-designer_mainwindow") )
		{
			connect( this, SIGNAL( getMd( aCfg ** ) ), widget, SLOT( getMd( aCfg ** ) ));
			emit ( getMd( &md ) );
		}
	}
	return md;
}



/*!
 *	Function init. Sets metadata object and tables object.
 */
void
wDBTable::init()// aDatabase *adb )
{
	qulonglong id=0;
	aCfgItem o;
	aCfgItem mditem, docitem;

	m_table = 0;

	md = getMd();
	id = getId();
	aLog::debug("wDBTable::init(), metadata object id="+QString::number(id));
	if ( md && id != 0)
	{
		o = md->find(id);
		QString objClass = md->objClass(o);
		if(objClass==md_document)
		{
			o = md->findChild(o,md_tables); // object tables
			tables = o;
		}
		if(objClass==md_journal){
			o = md->findChild(o, md_columns);
			tables = o;
		}
		if(objClass==md_catalogue)
		{
			o = md->findChild(o,md_element);
			tables = o;
		}
	}
	else
	{
		aLog::debug("wDBTable::init(), Can't get metadata or metaobject Id");
	}

}



/*!
 * \en	Sets available tables, using object tables. \_en
 */
void
wDBTable::setAvailableTables()
{
aCfgItem o_table, o = tables;
QString str;
QStringList listIdTable;
int res;
QList<int> vList = getBindList();

	if(o.isNull()) return;
	QString objClass = md->objClass(o);
	list_available_tables.clear();
	if(objClass==md_tables)
	{
		res = md->countChild(o,md_table); // ind kol_vo tables in obj tables
		for(int i=0; i<res; i++)
		{
			o_table = md->findChild(o,md_table,i);
			listIdTable << QString("%1").arg(md->id(o_table));
		}
	}
	if(objClass==md_columns)
	{
		list_available_tables << tr("Available columns");
	}
	if(objClass==md_element)
	{
		list_available_tables << md_element;
	}

	res = listIdTable.count();
	for(int i=0; i<res; i++)
	{
		if(vList.contains(listIdTable[i].toInt()))
			str ="* ";
		else
			str ="";
		list_available_tables << str + md->attr(md->find(listIdTable[i].toInt()),mda_name);
	}
}




/*!
 * \en 	Handler line current change signal. \_en
 */
void
wDBTable::lineUpdate( aTableOp mode)
{
QSqlRecord *rec = currentRecord();
	switch(mode)
	{
		case aOpUpdate:
		break;
		case aOpInsert:
		break;
		case aOpDelete:
		emit(deleteLine(rec));
		return;
		default:
		break;
	}
	emit(saveLine(rec));
}



/*!
 *	Return system icon for display object record state.
 */
QPixmap
wDBTable::systemIcon()
{
        aWidget *container = aWidget::parentContainer( this );
        QString ctype="";
        QPixmap pm;
        aDataTable *r = m_table;
        int df=0, cf=0, mf=0;

        if ( container ) ctype = container->metaObject()->className();
        if ( r )
	{
                if ( r->contains("df") ) df = r->field("df").value().toInt();
                if ( r->contains("cf") ) cf = r->field("cf").value().toInt();
		else
		{
			if(ctype=="wJournal")
			{
				aDocJournal* sysObj = new aDocJournal(db);
				if(sysObj)
				{
					if(sysObj->findDocument(r->field("id").value().toULongLong()))
					{
						aDocument *doc = sysObj->CurrentDocument();
						cf = doc->IsConducted();
						delete doc;
					}
					else
					{
						printf(">>doc select failed!\n");
					}
				}

				delete sysObj;


			}
		}
                if ( r->contains("mf") ) mf = r->field("mf").value().toInt();
		if(ctype=="wJournal")
		{
                        pm = t_doc;
                        if ( df ) pm = t_doc_d;
                        if ( cf && !df ) pm = t_doc_t;
                        if ( mf && !df ) pm = t_doc_m;
                        if ( cf && mf && !df ) pm = t_doc_tm;
                }
		else
	                if(ctype=="wCatalogue")
                	{
                	        pm = t_cat_e;
				if ( df ) pm = t_cat_ed;
                	}
			else
				if(ctype=="wCatGroupe")
				{
					pm = t_cat_g;
	        	                if ( df ) pm = t_cat_gd;
				}
        }
        return pm;
}



/*!
 *	Sets property `DefFields', `DefHeaders', `ColWidth', `DefIdList' to
 *	values from metadata.
 */
void
wDBTable::setFields(int idTable)
{
	int field_count,j;
	int i;
	QString str;
	QStringList Cwidth, list_fields,list_id;
	aCfgItem o, o_table, o_field;
	QString mdtag=QString(md_field);

	list_fields.clear();
	o = tables; // object tables
	QString objClass = md->objClass(o);
	if(objClass==md_tables)
	{
		o_table = md->find(idTable);
	}
	else
	if(objClass==md_columns)
	{
		o_table = o;
		mdtag=QString(md_column);
	}
	else
	if(objClass==md_element)
	{
		o_table = o;
	}
	colWidth.clear();
	i=0;
	while(i<numCols())
	{
		removeColumn(0);
	}
	if(!o_table.isNull())
	{
		field_count = md->countChild(o_table,mdtag);
		for (j=0; j<field_count; j++)
		{
			o_field = md->findChild(o_table,mdtag,j);
			list_fields << md->attr(o_field,mda_name);
			list_id << md->attr(o_field,mda_id);
			str.setNum(j);
			QString fld = md->attr(o_field,"name");
			addColumn(fld, fld, property("DefaultColWidth").toInt());
			Cwidth << property("DefaultColWidth").toString();
		}
	}
	setProperty("DefFields",list_fields);
	setProperty("DefHeaders",list_fields);
	setProperty("ColWidth",Cwidth);
	setProperty("DefIdList",list_id);
}



/*!
 *	Gets table id at position table.
 */
int
wDBTable::getTableId(int numTable)
{
	int res = -1;
	aCfgItem o, o_table;
	if ( numTable == -1 ) return res;
	o = tables; // object tables
	QString objClass = md->objClass(o);
	if(objClass==md_tables)
	{
		o_table = md->findChild(o,md_table,numTable);
		res = md->id(o_table);
	}
	if(objClass==md_columns)
	{
		res = md->id(o);
	}
	if(objClass==md_element)
	{
		res = md->id(o);
	}
	return res;
}



/*!
 *	Gets table position at id table.
 */
int
wDBTable::getTableInd(int id)
{
int j,tableCount;
aCfgItem o, o_table;
	o = tables; // object tables
	QString objClass = md->objClass(o);
	if(objClass==md_columns || objClass==md_element)
	{
		return tableInd;
	}
	tableCount = md->count(o,md_table);
	for(j=tableCount-1; j>=0; j--)
	{
		o_table = md->findChild(o,md_table,j);
		if(md->id(o_table)==id) break;
	}
return j;
}



/*!
 *	Gets list of id fields or list of name fields.
 */
QStringList
wDBTable::getFields(int idTable, bool GetId)
{
	QStringList lst;
	if (idTable==-1) {
		return lst;
	}

	int items_count,j;
	aCfgItem o, o_table, o_item;
	QString	mdtag=md_field;

	o = tables; // object tables
	QString objClass = md->objClass(o);
	if(objClass==md_tables)
	{
		o_table = md->find(idTable);
	}
	else
	if(objClass==md_columns)
	{
		o_table = o;
		mdtag = md_column;
	}
	else
	if(objClass==md_element)
	{
		o_table = o;
	}
	if(!o_table.isNull())
	{
		items_count = md->countChild(o_table,mdtag);
		for (j=0; j<items_count; j++)
		{
			o_item = md->findChild(o_table,mdtag,j);
			if(GetId)
			  lst << md->attr(o_item,mda_id);
			else
			  lst << md->attr(o_item,mda_name);
		}
	}
	else {
		debug_message("Table not found!\n");
	}
return lst;
}



/*!
 *	Gets field type.
 */
QString
wDBTable::getFieldType(long id)
{
	aCfgItem o;
	QString str="";

	QString objClass = md->objClass(tables);
	if(objClass==md_columns) {
		id= journalFieldId(id);
	}
	o = md->find(id);
	if(!o.isNull())
	{
		str = md->attr(o,mda_type);
	}
return str;
}



/*!
 *	Gets field name.
 */
QString
wDBTable::getFieldName(long id)
{
	aCfgItem o;
	QString str="";
	o = md->find(id);
	if(!o.isNull())
	{
		str = md->attr(o,mda_name);
	}
return str;
}



/*!
 *	Function check property name, id, header and width.
 */
void
wDBTable::checkFields()
{
	QStringList fl,hl,cl,il;
	unsigned int i;
	QString str;

	fl = property("DefFields").toStringList();
	il = property("DefIdList").toStringList();
	cl = property("ColWidth").toStringList();
	hl = property("DefHeaders").toStringList();
	for(i=0; i<il.count(); i++)
	{
		str = getFieldName(il[i].toInt());
		if(i<fl.count())
		{
			if(str!=fl[i])
			{
				aLog::print(aLog::Debug, QString("wDBTable unknown field name `%1' or (and) id `%1'\n").arg(str).arg(il[i]));
			}
		}
		else il.removeAt(i--);
		if(i>=hl.count()) hl << str;
		if(i>=cl.count()) cl << property("DefaultColWidth").toString();
	}
	while(i<hl.count())
	{
		hl.removeAt(i);
	}
	while(i<cl.count())
	{
		cl.removeAt(i);
	}
	setProperty("DefFields", fl );
	setProperty("DefHeaders", hl );
	setProperty("ColWidth", cl );
	setProperty("DefIdList", il );
}




/*!
 *	Initialisation the widget on form loaded in engine.
 */
void
wDBTable::init(aDatabase *adb, aEngine *e )
{

	aLog::print(aLog::Debug, tr("wDBTable init in engine "));
	unsigned int countField,i;
	aCfgItem o, own;
	QString str, ctype;
	QStringList lst,lstHead,lstWidth;
	int tid;
	aWidget *container = NULL;

	t_doc = rcIcon( "t_doc.png" );
        t_doc_d = rcIcon( "t_doc_d.png" );
        t_doc_t = rcIcon( "t_doc_t.png" );
        t_doc_m = rcIcon( "t_doc_m.png" );
        t_doc_tm = rcIcon( "t_doc_tm.png" );
	t_cat_e = rcIcon( "t_cat_e.png" );
        t_cat_ed = rcIcon( "t_cat_ed.png" );
        t_cat_g = rcIcon( "t_cat_g.png" );
        t_cat_gd = rcIcon( "t_cat_gd.png" );

	engine = e;
	db = adb;
	md = &adb->cfg;
	tid = property("TableInd").toInt();
	container = aWidget::parentContainer( this );
	if ( !container )
	{
		aLog::print(aLog::Error, tr("wDBTable not in Ananas object container "));
		return;
	}
	else
	{
		o = md->objTable( container->getId(), tid );
		if ( o.isNull() )
		{
			aLog::print(aLog::Error, tr("wDBTable init meta object not found "));
		}
		ctype = container->metaObject()->className();
		aLog::print(aLog::Info, tr("wDBTable container type is %1 ").arg(ctype));

		setContainerType(ctype);
	}

	if ( o.isNull() )
	{
		aLog::print(aLog::Error, tr("wDBTable init meta object not found "));
		return;
	}

	// clear all columns
	countField = numCols();
	for(i=0; i<countField;i++)
	{
		removeColumn(0);
	}

	aDataTable *tbl = NULL;
	if ( containerType() == "wDocument" )
	{
		QString flt;
		flt = QString("idd=%1").arg(container->uid());
		aLog::print(aLog::Info, tr("wDBTable filter is %1 ").arg(flt));
		tbl = new aDataTable( o, adb );
		tbl->setFilter(flt);
	}
	if ( containerType() == "wCatalogue" ) {
                tbl = container->table();
		tbl->setFilter(QString("idg=0"));
		newDataId(0);
		tbl->append( QSqlField("system_icon", QVariant::String) );
		tbl->setCalculated("system_icon", true );
          }
	if ( containerType() == "wJournal" ) {
		tbl = container->table();
		tbl->append( QSqlField( "system_icon", QVariant::String ) );
		tbl->setCalculated( "system_icon", true );
        }
	setSqlCursor( tbl );
	if ( !tbl ) return;

	if ( containerType() == "wJournal" ) {
		addColumn( "system_icon", "", 20 );
		setColumnReadOnly( 0, true );
		if (md->objClass(*(container->getMDObject()))==md_journal && !((aDocJournal*) container->dataObject())->type() ) {
			addColumn( "ddate", tr("Date"), 100 );
			addColumn( "pnum", tr("Prefix"), 200 );
			addColumn( "num", tr("Number"), 100 );
		}
	}
	if ( containerType() == "wCatalogue" ) {
		addColumn( "system_icon", "", 20 );
		setColumnReadOnly( 0, true );
	}

	lst = property("DefIdList").toStringList();
	lstHead = property("DefHeaders").toStringList();
	lstWidth = property("ColWidth").toStringList();

	if (md->objClass(*(container->getMDObject()))!=md_journal || ((aDocJournal*) container->dataObject())->type() ) {
		for(i=0; i<(unsigned)lst.count();i++)
		{
			if ( containerType() == "wJournal" )
			{
				str = "uf"+QString::number(journalFieldId(lst[i].toLong()));
			}
			else
			{
				str = "uf"+lst[i];
			}
			addColumn(str,lstHead.value(i),lstWidth.value(i).toInt());
		}
	}

	tbl->select();
	tbl->first();
	refresh(RefreshAll);
	setWFieldEditor();
	aLog::print(aLog::Debug, tr("wDBTable init in engine ok"));
}



/*!
 *	Set custom field editor (wField) used property `value'.
 */
void
wDBTable::setWFieldEditor()
{
	 aEditorFactory * f = new  aEditorFactory(this);
	 f->setMd(md);
	 setItemDelegate( f );
}



/*!
 *	Construct cell editor. Creates new wField object and inited it.
 */
QWidget*
aEditorFactory::createEditor(QWidget * parent, const QStyleOptionViewItem &, const QModelIndex & index) const
{
wField * tmp;
QString str,stmp;
wField::tEditorType type = wField::Unknown;
	if(!m_table || !md) return 0;
	QString field = m_table->columnField( index.column() );
	if ( !field.startsWith("uf") ) return 0;
	str = field.mid(2);
	tmp  = new wField(parent,"");
	str = m_table->getFieldType(str.toInt());
	stmp = str.section(' ',0,0);
	if(stmp=="C") type = wField::String;
	if(stmp=="N") type = wField::Numberic;
	if(stmp=="D") type = wField::Date;
	if(stmp=="B") type = wField::Boolean;
	if(stmp=="O")
	{
		int tid;
		tid = str.section(' ',1,1).toInt();
		aCfgItem o = md->find(tid);
		if(!o.isNull())
		{
			str = md->objClass(o);
			if(str == md_catalogue)
				type = wField::Catalogue;
			if(str == md_document)
				type = wField::Document;

		}
		else
		{
			aLog::print(aLog::Error,tr("aEditorFactory field metaobject not found"));
		}
	}
	tmp->setFieldType(m_table->getFieldType(field.mid(2).toInt()));
	tmp->setEditorType(type);
	tmp->initObject( m_table->db );
	tmp->engine = m_table->engine;
	// A custom editor does not fill its background (QWidget is transparent in
	// Qt4/6), so the item text underneath would show through the editor.  Make
	// the cell editor opaque.
	tmp->setAutoFillBackground( true );
	tmp->setBackgroundRole( QPalette::Base );
	return tmp;
}

/*!
 *	Fill the editor with the cell value.  For a reference column the cell shows
 *	the object name but the editor must be initialized with the stored id, so
 *	the id is taken straight from the data table.
 */
void
aEditorFactory::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	wField *field = qobject_cast<wField *>(editor);
	aDataTable *t = m_table ? m_table->sqlCursor() : 0;
	if ( field && t ) {
		QString colField = m_table->columnField( index.column() );
		if ( !colField.isEmpty() && t->sysFieldExists( "text_" + colField ) ) {
			t->seek( index.row() );
			field->setValue( t->sysValue( colField ).toString() );
			return;
		}
	}
	QStyledItemDelegate::setEditorData( editor, index );
}
void
aEditorFactory::setMd(aCfg * cfg)
{
	md = cfg;
}

aEditorFactory::aEditorFactory( wDBTable *table )
	: QStyledItemDelegate( table )
{
	m_table = table;
	md = 0;
}



/*!
 *	Gets list of id binding table.
 */
QList<int>
wDBTable::getBindList()
{
QObjectList wList;
int id;
wDBTable* wtable;
QObject* wd = aWidget::parentContainer( this );
	listBindings.clear();
    	wList = aQueryList(wd, "wDBTable");
	QListIterator<QObject*> it( wList ); // iterate over the wDBTable
	while ( it.hasNext() )
	{
		wtable = qobject_cast<wDBTable*>( it.next() );

		if(strcmp(wtable->objectName().toLatin1(),this->objectName().toLatin1())) // don't added current id
		{
		   if(strncmp("qt_dead_widget_",wtable->objectName().toLatin1(),strlen("qt_dead_widget_")))
		   {
			id = wtable->property("TableInd").toInt();
			if(id>=0)
			{
				listBindings << id;
			}
		   }
		}
	}
return listBindings;
}



/*!
 *	Get field value slot. Don't implemented.
 */
QVariant
wDBTable::Value( const QString &colname )
{
	aLog::print(aLog::Error, tr("wDBTable: function Value() call"));
	return QVariant("");
}



/*!
 *	Emit signal on select record.
 */
void
wDBTable::lineChange(int, int)
{
	QSqlRecord * rec = currentRecord();
	if ( !rec ) return;
	qulonglong id = 0;
	if(rec->contains("id")) id = rec->value("id").toLongLong();
	aLog::print(aLog::Info, tr("wDBTable: select document %1").arg(id));
	emit( selectRecord( id ) );
}


/*!
 *	Sets idd system field for newly added document table row.
 */
void
wDBTable::lineInsert(QSqlRecord* rec){

	if (containerType() == "wDocument")
	{
		if(rec->contains("idd")) rec->setValue("idd",QVariant(doc_id));
		if(rec->contains("ln")) rec->setValue("ln",numRows()-1);
	}
	if(containerType() == "wCatalogue")
	{
		if(rec->contains("idg")) rec->setValue("idg",QVariant(cat_group_id));
	}
}


/*!
 *	Delete line function.
 */
bool
wDBTable::deleteCurrent()
{
	if ( !m_table ) return false;
	m_table->seek( currentRow() );
	m_table->Delete();
	refresh();
	emit(updateCurr(currentRow(),currentColumn()));
	return true;
}


/*!
 *	Key press handler.
 */
void
wDBTable::keyPressEvent ( QKeyEvent *e )
{
	qulonglong id;

	aWidget *container = NULL;
	if ( searchMode == false && !e->text().isEmpty() && e->text().at( 0 ).isPrint() )
	{
		searchOpen( e->text() );
	}
	else
	{
		searchClose();
	}

	if(containerType() =="wJournal")
	{
		e->ignore();
	}
	if(containerType() =="wCatalogue")
	{
		switch ( e->key() )
		{
		case Qt::Key_Escape:
			e->ignore();
			break;
		case Qt::Key_Return:
			if(currentRecord())
			{
				id = currentRecord()->value(0).toLongLong();
				if ( e->modifiers() == Qt::ShiftModifier )
				{
					EditElement();
				} else
				{
					emit( selected( id ) );
				}
				e->accept();
			}
			else
			{
				aLog::print(aLog::Info, tr("wDBTable: current record not setted"));
			}
			break;
		default:
			e->ignore();
			break;
		}
	}
	QTableWidget::keyPressEvent( e );
}



/*!
 *	Slot processes document id change, setting new filter.
 */
void
wDBTable::newFilter(const QString & flt)
{
	setFilter(flt);
	refresh();
}

/*!
 *	Slot remembers new document id.
 */
void
wDBTable::newDataId(const qulonglong id)
{
	if(containerType() == "wDocument")
	{
		setId(id);
	}
	if(containerType() == "wCatalogue")
	{
		cat_group_id = id;
	}
}


/*!
 *	Opens form for edit catalogue element.
 */
void
wDBTable::EditElement()
{

	ANANAS_UID id = 0;
	aForm * f = 0;

	id = currentRecord()->value(0).toLongLong();
	if ( id ) {
		if ( engine ) {
			f = engine->openForm( aWidget::parentContainer( this )->getId(), 0, md_action_edit, md_form_elem, id, ( aWidget *) this );
			if ( f ) {
				connect(f, SIGNAL( update( ANANAS_UID )), this, SLOT(updateItem( ANANAS_UID )));
			}
		}
	}
}


long
wDBTable::journalFieldId(long columnId){
	aCfgItem item;

	item= md->find(md->find(columnId),md_fieldid);
	return md->text(item).toLong();

}

QString
wDBTable::journalFieldName(long columnId)
{
	aCfgItem item;
	item= md->find(md->find(columnId),md_fieldid);
	item = md->find(md->text(item).toLong());
	if(!item.isNull())
	{
		QString s = md->attr(item,mda_type);
		QChar ch = s[0];
		if(ch.toUpper()=='O')
		{
			return QString("text_uf%1").arg(md->attr(item,mda_id));
		}
		else
		{
			return QString("uf%1").arg(md->attr(item,mda_id));
		}
	}
	return "uf0";

}

/*!
 *	Handler table cell end edit.
 */
void
wDBTable::updateTableCellHandler(int r, int c)
{
	if ( m_populating ) return;
	lastEditedRow= r;
	lastEditedCol= c;
	if ( m_table && c >= 0 && c < m_columns.size() ) {
		QTableWidgetItem *it = item( r, c );
		if ( it ) {
			m_table->seek( r );
			m_table->setSysValue( m_columns.at(c), it->text() );
			m_table->Update();
			// The editor stores the raw value (a reference column keeps the
			// object id), so show the display form again (the object name).
			m_populating = true;
			it->setText( displayValue( m_columns.at(c) ) );
			m_populating = false;
		}
	}
}

/*!
 *	Reimplemented update. Emits updateCurr().
 */
bool
wDBTable::updateCurrent()
{
	bool res = false;
	if ( m_table ) {
		m_table->seek( currentRow() );
		m_table->Update();
		res = true;
		emit(updateCurr(lastEditedRow, lastEditedCol));
	}
	return res;
}

/*!
 *	Commit the value of the currently open cell editor to the model/data table
 *	and close it.  The editor is normally committed when it loses focus, but a
 *	wField editor keeps the focus on a child (focus proxy), so clicking a form
 *	button does not commit it and the last edited value would be lost.
 */
void
wDBTable::commitEditor()
{
	if ( state() != QAbstractItemView::EditingState ) return;
	QWidget *editor = viewport()->findChild<wField*>();
	if ( !editor ) return;
	commitData( editor );
	closeEditor( editor, QAbstractItemDelegate::SubmitModelCache );
}


/*!
 *	Double click handler.
 */
void
wDBTable::doubleClickEventHandler(int /*rol*/, int /*col*/)
{
	if(containerType() =="wCatalogue" || containerType() == "wJournal")
	{
		if(currentRecord())
		{
			qulonglong id = currentRecord()->value(0).toLongLong();
			emit( selected( id ) );
		}
	}

}

/**
 *	Confirm edit dialog (delete only).
 */
bool
wDBTable::confirmEdit( aTableOp m ) {
	if ( m == aOpDelete ) {
		return 0 == QMessageBox::question(
            this,
            tr("Remove record?"),
            tr("You are going to remove record <br>"
                "Are you sure?"),
            tr("&Yes, remove"), tr("&No"),
            QString(), 0, 1 );
	}
	return true;
}

/**
 *	Inserts a new row (always at the end).
 */
bool
wDBTable::beginInsert() {
	if ( !m_table || isReadOnly() || !numCols() )
		return false;
	if ( !m_table->canInsert() )
		return false;

	m_table->insertBuffer();
	lineInsert( m_table->editBuffer() );
	m_table->insert();
	refresh();
	setCurrentCell( numRows()-1, 0 );
	return true;

}


/*!
 *	Context menu handler.
 */
void
wDBTable::contextMenuEvent ( QContextMenuEvent * e )
{
	QString str, ctype;

	if ( containerType() == "wDocument" || containerType() == "wCatalogue" ) {
		QMenu popupForDoc( this );
		QAction *actInsert = popupForDoc.addAction( tr( "New" ) );
		QAction *actUpdate = popupForDoc.addAction( tr( "Edit" ) );
		QAction *actDelete = popupForDoc.addAction( tr( "Delete" ) );

		if ( !m_table || isReadOnly() || !numCols() ) {
			actInsert->setEnabled( false );
			actUpdate->setEnabled( false );
			actDelete->setEnabled( false );
		}

		QAction *r = popupForDoc.exec( e->globalPos() );
		if(r==actInsert) {
			beginInsert();
		} else if(r==actUpdate) {
			keyPressEvent( new QKeyEvent( QEvent::KeyPress, Qt::Key_F2, Qt::NoModifier));
		} else if(r==actDelete) {
			deleteCurrent();
		}
	}


	if ( containerType() == "wJournal" )
	{
		QMenu popup( this );
		QAction *actInsert = popup.addAction( tr( "New" ) );
		QAction *actUpdate = popup.addAction( tr( "Edit" ) );
		QAction *actDelete = popup.addAction( tr( "Delete" ) );
		QAction *actView = popup.addAction( tr( "View" ) );
		QAction *actRefresh = popup.addAction( tr( "Refresh" ) );
		QAction *r = popup.exec( e->globalPos() );
		if(r==actInsert)
			emit(insertRequest());
		else
			if(r==actUpdate)
				emit(updateRequest());
			else
				if(r==actDelete)
					emit(deleteRequest());
				else
					if(r==actView)
						emit(viewRequest());
						if(r==actRefresh)
							{
								refresh();
							}
	}
	e->accept();

}


void
wDBTable::updateItem( ANANAS_UID db_uid )
{
	Q_UNUSED(db_uid);
	refresh();
	emit currentChanged( currentRecord() );
}


int
wDBTable::Select( ANANAS_UID db_uid )
{
	aDataTable *t = m_table;
	if ( !t ) return 0;

	ANANAS_UID cur_id = 0;
	int curr = currentRow(), curc=currentColumn(), row = 0;
	bool found = false;

	while ( t->seek( row ) ){
		cur_id = t->sysValue( "id" ).toULongLong();
		if ( cur_id == db_uid ) {
			found = true;
			break;
		}
		row++;
	}
	if ( found ) {
		setCurrentCell( row, curc );
	} else setCurrentCell( curr, curc );
	return 0;
}


bool
wDBTable::searchColumn( const QString & text, bool FromCurrent, bool Forward )
{

	QString s;
	int curr = currentRow(), curc=currentColumn(), row = 0;
	bool found = false;
	aDataTable *t = m_table;
	if ( !t ) return false;

	if ( FromCurrent ) row = curr;
	if ( Forward ) row++; else row--;
	QString field = m_columns.value( curc );

	while ( t->seek( row ) ){
		s = t->sysValue( field ).toString();
		if ( s.left( text.length() ) == text ) {
			found = true;
			break;
		}
		if ( Forward ) row++; else row--;
	}
	if ( found ) {
		setCurrentCell( row, curc );
	};
	return found;
}



void
wDBTable::searchOpen( const QString & text )
{
	searchWidget = new aSearchWidget( aWidget::parentContainer( this ), this );
	searchMode = true;
	searchWidget->setFocus();
	searchWidget->search( text );
}


void
wDBTable::searchClose()
{
	if ( searchWidget ) {
		setFocus();
		searchWidget->deleteLater();
		searchMode = false;
		searchWidget = 0;
	}
}


/* ----------------------------------------------------------------------- */
/* QTableWidget based helpers                                               */
/* ----------------------------------------------------------------------- */

void
wDBTable::addColumn( const QString &field, const QString &header, int width )
{
	int c = columnCount();
	setColumnCount( c + 1 );
	m_columns << field;
	setHorizontalHeaderItem( c, new QTableWidgetItem( header.isEmpty() ? field : header ) );
	if ( width > 0 ) setColumnWidth( c, width );
}

void
wDBTable::removeColumn( int col )
{
	if ( col < 0 || col >= columnCount() ) return;
	QTableWidget::removeColumn( col );
	if ( col < m_columns.size() ) m_columns.removeAt( col );
}

void
wDBTable::setFilter( const QString &flt )
{
	if ( m_table ) m_table->setFilter( flt );
}

void
wDBTable::refreshAll()
{
	refresh( RefreshAll );
}

QVariant
wDBTable::value( int row, int col )
{
	if ( !m_table ) return QVariant();
	QString field = m_columns.value( col );
	if ( field.isEmpty() ) return QVariant();
	m_table->seek( row );
	return m_table->sysValue( field );
}

void
wDBTable::setColumnReadOnly( int col, bool ro )
{
	for ( int row = 0; row < rowCount(); ++row ) {
		QTableWidgetItem *it = item( row, col );
		if ( !it ) continue;
		Qt::ItemFlags f = it->flags();
		if ( ro ) f &= ~Qt::ItemIsEditable;
		else f |= Qt::ItemIsEditable;
		it->setFlags( f );
	}
}

void
wDBTable::setReadOnly( bool ro )
{
	if ( m_table ) m_table->setReadOnly( ro );
	setEditTriggers( ro ? QAbstractItemView::NoEditTriggers : QAbstractItemView::DoubleClicked );
}

bool
wDBTable::isReadOnly() const
{
	return m_table ? m_table->isReadOnly() : true;
}

QSqlRecord*
wDBTable::currentRecord()
{
	if ( !m_table ) return 0;
	m_table->seek( currentRow() );
	return m_table->currentRecord();
}

QString
wDBTable::displayValue( const QString & field ) const
{
	if ( !m_table ) return QString();
	// Reference columns keep the object id in `ufNNN`; the human-readable
	// name is the calculated `text_ufNNN` field.
	if ( m_table->sysFieldExists( "text_" + field ) )
		return m_table->sysValue( "text_" + field ).toString();
	QVariant v = m_table->sysValue( field );
	if ( m_table->field( field ).type() == QVariant::DateTime && v.isValid() )
		return v.toDate().toString();
	return v.toString();
}

void
wDBTable::refresh( int mode )
{
	if ( !m_table ) return;

	if ( mode == RefreshColumns ) {
		m_columns.clear();
		setColumnCount( 0 );
		for ( int i = 0; i < m_table->count(); i++ )
			addColumn( m_table->fieldName(i), m_table->fieldName(i), defColWidth );
		return;
	}

	m_populating = true;
	int rows = m_table->size();
	setRowCount( rows );
	for ( int r = 0; r < rows; ++r ) {
		m_table->seek( r );
		for ( int c = 0; c < columnCount(); ++c ) {
			QString field = m_columns.value( c );
			QTableWidgetItem *it = item( r, c );
			if ( !it ) {
				it = new QTableWidgetItem();
				setItem( r, c, it );
			}
			if ( field == "system_icon" ) {
				it->setText( "" );
				it->setIcon( QIcon( systemIcon() ) );
			} else {
				it->setText( displayValue( field ) );
			}
			if ( !isReadOnly() )
				it->setFlags( it->flags() | Qt::ItemIsEditable );
		}
	}
	m_populating = false;
}


/*!
 *	Interactive search by first letters of the current column.
 */
aSearchWidget::aSearchWidget( QWidget *parent, wDBTable *table )
: QFrame( parent )
{
	t = table;
	ftext = "";
	setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
	setFocusPolicy( Qt::StrongFocus );
	new QHBoxLayout( this );
	l = new QLineEdit( this );
        l->installEventFilter( this );
	setFocusProxy( l );
	layout()->addWidget( l );
	move( 3+t->x()+t->columnViewportPosition( t->currentColumn()), t->y()+1);
	resize( t->columnWidth( t->currentColumn() )-2, 25 );
	connect( l, SIGNAL( textChanged( const QString & ) ), this, SLOT( setText( const QString & ) ) );
}


aSearchWidget::~aSearchWidget()
{
}


void
aSearchWidget::search( const QString &t )
{
	show();
	l->setText( t );
}


void
aSearchWidget::setText( const QString &text )
{
	if ( t->searchColumn( text ) ) ftext = text;
	else l->setText( ftext );
}


bool
aSearchWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if ( obj == l ) {
		if ( ev->type() == QEvent::FocusOut ) {
			t->searchClose();
			return true;
		}
		if ( ev->type() == QEvent::KeyPress ) {
		QKeyEvent *e = ( QKeyEvent *) ev;
			switch ( e->key() ){
			case Qt::Key_Return:
			case Qt::Key_Escape:
				t->searchClose();
				break;
			case Qt::Key_Up:
				t->searchColumn( ftext, true, false );
				break;
			case Qt::Key_Down:
				t->searchColumn( ftext, true, true );
				break;
			default:
				return false;
				break;
			}
                return true;
            } else {
                return false;
            }
        } else {
            return QFrame::eventFilter( obj, ev );
        }
}
