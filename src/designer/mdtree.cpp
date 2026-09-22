/****************************************************************************
** $Id: mdtree.cpp,v 1.1 2008/11/05 21:16:27 leader Exp $
**
** Code file of the Metadata Tree of Ananas Designer applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
**
** This file is part of the Designer application  of the Ananas
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

#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QStatusBar>
#include <QLineEdit>
#include <QMessageBox>
#include <QImage>
#include <QBitmap>
#include <QFont>
#include <QCursor>
#include <QLayout>
#include <QFileDialog>
#include <QMenu>
#include <QDir>
#include <QMdiArea>

#include "adatabase.h"
#include "mainform.h"
#include "deditfield.h"
#include "deditcfg.h"
#include "deditdoc.h"
#include "deditreport.h"
#include "deditdialog.h"
#include "deditcat.h"
#include "deditireg.h"
#include "deditareg.h"
#include "deditjournal.h"
#include "deditwebform.h"
#include "deditcolumns.h"
#include "awindowslist.h"

//#include "acfg.h"
#include "acfgrc.h"

#include "mdtree.h"

extern MainForm *mainform;
extern QPixmap rcIcon(const char *name);
//extern void set_Icon(QTreeWidgetItem *item, const char *name);



aListViewItem::aListViewItem(ananasListViewItem *parent, ananasListViewItem *after, aCfg *cfgmd,  aCfgItem cfgobj, const QString &name )
: ananasListViewItem( parent, after, cfgmd, cfgobj, name )
{
	loadTree();
}



aListViewItem::aListViewItem(QTreeWidget *parent, aCfg *cfgmd,  aCfgItem cfgobj, const QString &name )
: ananasListViewItem( parent, cfgmd, cfgobj, name )
{
	loadTree();
}


aListViewItem::~aListViewItem()
{

}


QString
aListViewItem::text( int column ) const
{
	return QTreeWidgetItem::text( column );
}


/*!
Update metadata tree for the object context.
*/
void
aListViewItem::update()
{

}


void
aListViewItem::loadTree()
{
	aListViewItem *item, *nextitem;
	QString oclass, ldclass="";
	int i, n;
	aCfgItem cobj;

	// clear tree
	clearTree();
	if ( !md ) return;
	oclass = md->objClass( obj );
	if ( oclass == md_metadata ){
		cobj = md->find ( mdc_metadata );
		setIcon(0, QIcon(rcIcon("project.png")));
		setExpanded(true);
		cobj = md->find ( obj, md_catalogues, 0 );
		if ( cobj.isNull() )
		{
			cobj = md->insert( obj, md_catalogues, QString(), -1 );
		}
		new aListViewItem( this, getLastChild(), md, cobj, QObject::tr("Catalogues") );
		cobj = md->find ( obj, md_documents, 0 );
		if ( cobj.isNull() )
		{
			cobj = md->insert( obj, md_documents, QString(), -1 );
		}
		new aListViewItem( this, getLastChild(), md, cobj, QObject::tr("Documents") );
		cobj = md->find ( obj, md_reports, 0 );
		if ( cobj.isNull() )
		{
			cobj = md->insert( obj, md_reports, QString(), -1 );
		}
		new aListViewItem( this, getLastChild(), md, cobj, QObject::tr("Reports") );
		cobj = md->find ( obj, md_journals, 0 );
		if ( cobj.isNull() )
		{
			cobj = md->insert( obj, md_journals, QString(), -1 );
		}
		new aListViewItem( this, getLastChild(), md, cobj, QObject::tr("Journals") );
		cobj = md->find ( obj, md_registers, 0 );
		if ( cobj.isNull() )
		{
			cobj = md->insert( obj, md_registers, QString(), -1 );
		}
		cobj = md->find ( obj, md_iregisters, 0 );
		if ( cobj.isNull() )
		{
			cobj = md->insert( md->find (obj, md_registers, 0 ), md_iregisters, QString(), -1 );
		}
		new aListViewItem( this, getLastChild(), md, cobj, QObject::tr("Information registers") );
		cobj = md->find ( obj, md_aregisters, 0 );
		if ( cobj.isNull() )
		{
			cobj = md->insert( md->find (obj, md_registers, 0 ), md_aregisters, QString(), -1 );
		}
		new aListViewItem( this, getLastChild(), md, cobj, QObject::tr("Accumulation registers") );
	}
	if ( oclass == md_catalogues ){
		setIcon(0, QIcon(rcIcon("cat_g.png")) );
		ldclass = md_catalogue;
	}
	if ( oclass == md_documents ){
		setIcon(0, QIcon(rcIcon("doc_g.png")));
		ldclass = md_document;
	}
	if ( oclass == md_reports ){
		setIcon(0, QIcon(rcIcon("report_g.png")));
		ldclass = md_report;
	}
	if ( oclass == md_journals ){
		setIcon(0, QIcon(rcIcon("journ_g.png")));
		ldclass = md_journal;
	}
	if ( oclass == md_iregisters ){
		setIcon(0, QIcon(rcIcon("reg_g.png")));
		ldclass = md_iregister;
	}
	if ( oclass == md_aregisters ){
		setIcon(0, QIcon(rcIcon("regs_g.png")));
		ldclass = md_aregister;
	}
	if ( !ldclass.isEmpty() ) {
		n = md->count( obj, ldclass );
		for ( i = 0; i<n; i++ ) {
			cobj = md->find( obj, ldclass, i );
			if ( !cobj.isNull() ) {
				new aListViewItem( this, getLastChild(), md,  cobj, QString() );
			}
		}
	}

	if ( oclass == md_tables ){
		setIcon(0, QIcon(rcIcon("table_g.png")));
		return;
	}
	if ( oclass == md_forms ){
		setIcon(0, QIcon(rcIcon("form_g.png")));
		return;
	}
	if ( oclass == md_form ){
		setIcon(0, QIcon(rcIcon("form.png")));
		return;
	}
	if ( oclass == md_webforms ){
		setIcon(0, QIcon(rcIcon("webform_g.png")));
		return;
	}
	if ( oclass == md_webform ){
		setIcon(0, QIcon(rcIcon("webform.png")));
		return;
	}
	if ( oclass == md_table )
	{
		setIcon(0, QIcon(rcIcon("table.png")));
		return;
	}

// elements
	if ( oclass == md_catalogue ){
		setIcon(0, QIcon(rcIcon("cat.png")));
		loadCatalogue ();
		return;
	}
	if ( oclass == md_document ){
		setIcon(0, QIcon(rcIcon("doc.png")));
		loadDocument ();
		return;
	}
	if ( oclass == md_aregister ){
		setIcon(0, QIcon(rcIcon("regs.png")));
		loadARegister ();
		return;
	}
	if ( oclass == md_iregister ){
		setIcon(0, QIcon(rcIcon("reg.png")));
		loadIRegister ();
		return;
	}
	if ( oclass == md_journal ){
		setIcon(0, QIcon(rcIcon("journ.png")));
		loadJournal ();
		return;
	}
	if ( oclass == md_report ){
		setIcon(0, QIcon(rcIcon("report.png")));
		loadReport ();
		return;
	}
	if ( oclass == md_field ){
		setIcon(0, QIcon(rcIcon("field.png")));
		return;
	}
	if ( oclass == md_header )
	{
		setIcon(0, QIcon(rcIcon("doc_h.png")));
		return;
	}
	if ( oclass == md_element)
	{
		setIcon(0, QIcon(rcIcon("element.png")));
		return;
	}
    if( oclass == md_columns )
	{
		setIcon(0, QIcon(rcIcon("columns.png")));
		return;
	}
	if ( oclass == md_resources )
	{
		setIcon(0, QIcon(rcIcon("resourses.png")));
		return;
	}
	if ( oclass == md_dimensions )
	{
		setIcon(0, QIcon(rcIcon("dimensions.png")));
		return;
	}
	if ( oclass == md_information )
	{
		setIcon(0, QIcon(rcIcon("information.png")));
		return;
	}
	if (oclass == md_group)
	{
		setIcon(0, QIcon(rcIcon("group.png")));
		return;
	}
}

void
aListViewItem::loadDocument ()
{
	int		n, i;
	aCfgItem	cobj;
	aListViewItem	*tparent, *fparent;

	fparent = new aListViewItem( this, getLastChild(), md, md->find(obj, md_header, 0), QObject::tr("Header") );
	loadFields ( fparent );

	tparent = new aListViewItem( this, getLastChild(), md, md->find(obj, md_tables, 0), QObject::tr("Tables") );
	n = md->count ( obj, md_table );
	for ( i = 0; i < n; i++ )
	{
		cobj = md->find(  obj, md_table, i  );
		if ( !cobj.isNull() )
		{
			fparent = new aListViewItem( tparent, tparent->getLastChild(), md, cobj, QString() );
			loadFields ( fparent );
		}
	}
	loadForms ( this );
	loadWebForms ( this );
}

void
aListViewItem::loadFields (aListViewItem *parent)
{
	int		n, i;
	aCfgItem	cobj;

	n = md->count ( parent->obj, md_field );
	for ( i = 0; i < n; i++ )
	{
		cobj = md->find(  parent->obj, md_field, i  );
		if ( !cobj.isNull() )
		{
			new aListViewItem( parent, parent->getLastChild(), md, cobj, QString() );
		}
	}
}

void
aListViewItem::loadForms (aListViewItem *parent)
{
	int		n, i;
	aCfgItem	cobj, gobj;
	aListViewItem	*fparent;

	gobj = md->find(obj, md_forms, 0);
	if ( gobj.isNull() ) gobj = md->insert( obj, md_forms, QString(), -1 );
	fparent = new aListViewItem( parent, getLastChild(), md, gobj, QObject::tr("Forms") );
	n = md->count ( fparent->obj, md_form );
	for ( i = 0; i < n; i++ )
	{
		cobj = md->find(  fparent->obj, md_form, i  );
		if ( !cobj.isNull() )
		{
			new aListViewItem( fparent, getLastChild(), md, cobj, QString() );
		}
	}
}

void
aListViewItem::loadWebForms (aListViewItem *parent)
{
	int		n, i;
	aCfgItem	cobj, gobj;
	aListViewItem	*fparent;

	gobj = md->find(obj, md_webforms, 0);
	if ( gobj.isNull() ) gobj = md->insert( obj, md_webforms, QString(), -1 );
	fparent = new aListViewItem( parent, getLastChild(), md, md->find(obj, md_webforms, 0), QObject::tr("Web forms") );
	n = md->count ( fparent->obj, md_webform );
	for ( i = 0; i < n; i++ )
	{
		cobj = md->find(  fparent->obj, md_webform, i  );
		if ( !cobj.isNull() )
		{
			new aListViewItem( fparent, getLastChild(), md, cobj, QString() );
		}
	}
}

void
aListViewItem::loadJournal ()
{
	aListViewItem	*fparent;

	fparent = new aListViewItem( this, getLastChild(), md, md->find(obj, md_columns, 0), QObject::tr("Columns") );
	loadColumns( fparent );
	loadForms ( this );
	loadWebForms ( this );
}

void
aListViewItem::loadIRegister ()
{
	aListViewItem	*fparent;

	fparent = new aListViewItem( this, getLastChild(), md, md->find(obj, md_resources, 0), QObject::tr("Resources") );
	loadFields ( fparent );
	fparent = new aListViewItem( this, getLastChild(), md, md->find(obj, md_dimensions, 0), QObject::tr("Dimensions") );
	loadFields ( fparent );
	fparent = new aListViewItem( this, getLastChild(), md, md->find(obj, md_information, 0), QObject::tr("Information") );
	loadFields ( fparent );
}

void
aListViewItem::loadARegister ()
{
	aListViewItem	*fparent;

	fparent = new aListViewItem( this, getLastChild(), md, md->find(obj, md_resources, 0), QObject::tr("Resources") );
	loadFields ( fparent );
	fparent = new aListViewItem( this, getLastChild(), md, md->find(obj, md_dimensions, 0), QObject::tr("Dimensions") );
	loadFields ( fparent );
	fparent = new aListViewItem( this, getLastChild(), md, md->find(obj, md_information, 0), QObject::tr("Information") );
	loadFields ( fparent );
}

void
aListViewItem::loadCatalogue ()
{
	aListViewItem	*fparent;
	aCfgItem i;

	i = md->find(obj, md_element, 0);
	if ( i.isNull() ) i = md->insert( obj, md_element, QString(), -1 );
	fparent = new aListViewItem( this, getLastChild(), md, i, QObject::tr("Element") );
	loadFields ( fparent );
	i = md->find(obj, md_group, 0);
	if ( i.isNull() ) i = md->insert( obj, md_group, QString(), -1 );
	fparent = new aListViewItem( this, getLastChild(), md, i, QObject::tr("Group") );
	loadFields ( fparent );
	loadForms ( this );
	loadWebForms ( this );
}

void aListViewItem::loadReport ()
{
	loadForms ( this );
	loadWebForms ( this );
}

void
aListViewItem::loadColumns ( aListViewItem *parent )
{
	int i,n;
	aCfgItem	cobj;

	n = md->count ( parent->obj, md_column );
	for ( i =  0; i < n; i++)
	{
		cobj = md->find(  parent->obj, md_column, i  );
		if ( !cobj.isNull() )
		{
			new aListViewItem( parent, getLastChild(), md, cobj, QString() );
		}
	}
}


/*
*edit selected object
*/

void
aListViewItem::edit()
{
    QMdiArea *ws = mainform->ws;
    aWindowsList *wl = mainform->wl;
    QString oclass = md->objClass( obj );
    int objid = md->id( obj );
    if ( wl->find( objid ) ) {
	wl->get( objid )->setFocus();
	return;
    }

	if ( oclass == md_metadata )
	{
	    dEditCfg *e = new dEditCfg( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->setData( this );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	}
	if ( oclass == md_document)
	{
	    dEditDoc *e = new dEditDoc( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->setData( this );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	};
	if ( oclass == md_iregister)
	{
	    dEditIReg *e = new dEditIReg( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->setData( this );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	};
	if ( oclass == md_aregister)
	{
	    dEditAReg *e = new dEditAReg( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->setData( this );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	};
	if ( oclass == md_catalogue)
	{
	    dEditCat *e = new dEditCat( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->setData( this );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	};
	if ( oclass == md_field )
	{
	    dEditField *e = new dEditField( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->setData( this );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	};
	if ( oclass == md_report )
	{
	    dEditReport *e = new dEditReport( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->setData( this );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	};
	if ( oclass == md_journal )
	{
	    dEditJournal *e = new dEditJournal( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->setData( this );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	};
	if ( oclass == md_form )
	{
	    dEditDialog *e = new dEditDialog( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    e->setData( this );
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	};
	if ( oclass == md_webform )
	{
	    dEditWebForm *e = new dEditWebForm( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->setData( this );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	};
	if ( oclass == md_column )
	{
	    dEditColumns *e = new dEditColumns( ws, 0 );
	    e->setAttribute( Qt::WA_DeleteOnClose );
	    wl->insert( objid, e );
	    editor = e;
	    QObject::connect( mainform, SIGNAL( tosave() ), editor, SLOT( updateMD() ) );
	    e->setData( this );
	    e->show();
	    mainform->addTab(e);
	    e->parentWidget()->setGeometry(0,0,e->parentWidget()->frameSize().width(),
		e->parentWidget()->frameSize().height());
	    return;
	}
}

void aListViewItem::newObject()
{
	QString oclass = md->objClass( obj );
	if ( 	oclass == md_header || oclass == md_table ||
		oclass == md_element || oclass == md_group ||
		oclass == md_resources || oclass == md_dimensions || oclass == md_information ||
		oclass == md_columns )
			newField();
	if ( oclass == md_documents ) newDocument();
	if ( oclass == md_catalogues ) newCatalogue();
	if ( oclass == md_journals ) newJournal();
	if ( oclass == md_iregisters ) newIRegister();
	if ( oclass == md_aregisters ) newARegister();
	if ( oclass == md_reports ) newReport();
	if ( oclass == md_forms ) newForm();
	if ( oclass == md_webforms ) newWebForm();
	if ( oclass == md_tables ) newTable();
	if ( oclass == md_columns ) newColumn();
}

void aListViewItem::saveItem()
{

	//md->saveOneObject()
	QString oclass = md->objClass( obj );
	if ( 	oclass == md_field ||
		oclass == md_document ||
		oclass == md_catalogue ||
		oclass == md_journal ||
		oclass == md_iregister ||
		oclass == md_aregister ||
		oclass == md_report ||
		oclass == md_webform ||
		oclass == md_form ||
		oclass == md_table)
	{

		QString fname;
		QFileDialog fd( nullptr, QObject::tr("Save object") );
		fd.setFileMode(QFileDialog::AnyFile);
		fd.setNameFilter(QObject::tr("any files (*)"));
		fd.selectFile( oclass + md->attr(obj,mda_id) );
		if ( fd.exec() == QDialog::Accepted )
		{
			fname = QDir::toNativeSeparators(fd.selectedFiles().value(0));
			md->saveOneObject(obj,fname);
		}
	}
}

void aListViewItem::loadItem()
{

	QString oclass = md->objClass( obj );

	if ( 	oclass == md_header ||
		oclass == md_table ||
		oclass == md_element ||
		oclass == md_group ||
		oclass == md_resources ||
		oclass == md_dimensions ||
		oclass == md_information ||
		oclass == md_columns ||
		oclass == md_documents ||
		oclass == md_catalogues ||
		oclass == md_journals ||
		oclass == md_iregisters ||
		oclass == md_aregisters ||
		oclass == md_reports ||
		oclass == md_forms ||
		oclass == md_webforms ||
		oclass == md_tables ||
		oclass == md_columns )
	{

		QString fname;
		QFileDialog fd( nullptr, QObject::tr("Load object") );
		fd.setFileMode(QFileDialog::ExistingFiles);
		fd.setNameFilter(QObject::tr("any files (*)"));
		aCfgItem loadObj;
		if ( fd.exec() == QDialog::Accepted )
		{
			fname = QDir::toNativeSeparators(fd.selectedFiles().value(0));
			loadObj = md->loadOneObject(fname);
			if(loadObj.isNull())
			{
				return;
			}
			else
			{
				md->setAttr(loadObj, mda_name, QString("%1_copy").arg(md->attr(loadObj, mda_name)));
				QString loclass = md->objClass(loadObj);
				if ( loclass==md_field	&&
						(oclass == md_header || oclass == md_table ||
						oclass == md_element || oclass == md_group ||
						oclass == md_resources || oclass == md_dimensions ||
						oclass == md_information || oclass == md_columns ))
				{
					aCfgItem newobj = md->importCfgItem( obj, loadObj );
					aListViewItem *newitem = new aListViewItem( this, getLastChild(), md, newobj );
					Q_UNUSED(newitem);
				}
				if ( loclass==md_document &&  oclass == md_documents )
				{
					aCfgItem newobj = md->importCfgItem( obj, loadObj );
					aListViewItem *newitem = new aListViewItem( this, getLastChild(), md, newobj );
					Q_UNUSED(newitem);
				}
				if ( loclass==md_catalogue && oclass == md_catalogues )
				{
					aCfgItem newobj = md->importCfgItem( obj, loadObj );
					aListViewItem *newitem = new aListViewItem( this, getLastChild(), md, newobj );
					Q_UNUSED(newitem);
				}
				if ( loclass==md_journal && oclass == md_journals )
				{
					aCfgItem newobj = md->importCfgItem( obj, loadObj );
					aListViewItem *newitem = new aListViewItem( this, getLastChild(), md, newobj );
					Q_UNUSED(newitem);
				}
				if ( loclass==md_iregister && oclass == md_iregisters )
				{
					aCfgItem newobj = md->importCfgItem( obj, loadObj );
					aListViewItem *newitem = new aListViewItem( this, getLastChild(), md, newobj );
					Q_UNUSED(newitem);
				}
				if ( loclass==md_aregister &&  oclass == md_aregisters )
				{
					aCfgItem newobj = md->importCfgItem( obj, loadObj );
					aListViewItem *newitem = new aListViewItem( this, getLastChild(), md, newobj );
					Q_UNUSED(newitem);
				}
				if ( loclass==md_report && oclass == md_reports )
				{
					aCfgItem newobj = md->importCfgItem( obj, loadObj );
					aListViewItem *newitem = new aListViewItem( this, getLastChild(), md, newobj );
					Q_UNUSED(newitem);
				}
				if ( loclass==md_form &&  oclass == md_forms )
				{
					aCfgItem newobj = md->importCfgItem( obj, loadObj );
					aListViewItem *newitem = new aListViewItem( this, getLastChild(), md, newobj );
					Q_UNUSED(newitem);
				}
				if ( loclass==md_webform && oclass == md_webforms )
				{
					aCfgItem newobj = md->importCfgItem( obj, loadObj );
					aListViewItem *newitem = new aListViewItem( this, getLastChild(), md, newobj );
					Q_UNUSED(newitem);
				}
				if ( loclass==md_table &&  oclass == md_tables )
				{
					aCfgItem newobj = md->importCfgItem( obj, loadObj );
					aListViewItem *newitem = new aListViewItem( this, getLastChild(), md, newobj );
					Q_UNUSED(newitem);
				}
			}
		}
	}
}


void
aListViewItem::newField()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	QString oclass = md->objClass( obj );
	if (oclass == md_header || oclass == md_table ||
		oclass == md_element || oclass == md_group ||
		oclass == md_resources || oclass == md_dimensions || oclass == md_information)
	{
		setSelected( false );
		setExpanded( true );
		newobj = md->insert( obj, md_field, QObject::tr("New field") );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->edit();
	}
}


void
aListViewItem::newDocument()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	if ( md->objClass( obj ) == md_documents ) {
		setSelected( false );
		setExpanded( true );
		newobj = md->insertDocument(  QObject::tr("New document") );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->setExpanded( true );
		newitem->edit();
	}
}


void
aListViewItem::newCatalogue()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	if ( md->objClass( obj ) == md_catalogues ) {
		setSelected( false );
		setExpanded( true );
		newobj = md->insertCatalogue( QObject::tr("New catalogue") );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->setExpanded( true );
		newitem->edit();
	}
}

void
aListViewItem::newJournal()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	if ( md->objClass( obj ) == md_journals ) {
		setSelected( false );
		setExpanded( true );
		newobj = md->insertJournal( QObject::tr("New journal") );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->setExpanded( true );
		newitem->edit();
	}
}

void
aListViewItem::newIRegister()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	if ( md->objClass( obj ) == md_iregisters )
	{
		setSelected( false );
		setExpanded( true );
		newobj = md->insertIRegister(  QObject::tr("New information register") );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->setExpanded( true );
		newitem->edit();
	}
}

void
aListViewItem::newARegister()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	if ( md->objClass( obj ) == md_aregisters )
	{
		setSelected( false );
		setExpanded( true );
		newobj = md->insertARegister( QObject::tr("New accumulation register") );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->setExpanded( true );
		newitem->edit();
	}
}

void
aListViewItem::newReport()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	if ( md->objClass( obj ) == md_reports )
	{
		setSelected( false );
		setExpanded( true );
		newobj = md->insert( obj, md_report, QObject::tr("New report") );
		md->insert( newobj, md_forms, QString(), -1 );
		md->insert( newobj, md_webforms, QString(), -1 );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->setExpanded( true );
		newitem->edit();
	}
}

void
aListViewItem::newForm()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	if ( md->objClass( obj ) == md_forms )
	{
		setSelected( false );
		setExpanded( true );
		newobj = md->insert( obj, md_form, QObject::tr("New form") );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->setExpanded( true );
		newitem->edit();
	}
}

void
aListViewItem::newWebForm()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	if ( md->objClass( obj ) == md_webforms )
	{
		setSelected( false );
		setExpanded( true );
		newobj = md->insert( obj, md_webform, QObject::tr("New web form") );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->setExpanded( true );
		newitem->edit();
	}
}

void
aListViewItem::newTable()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	if ( md->objClass( obj ) == md_tables )
	{
		setSelected( false );
		setExpanded( true );
		newobj = md->insert( obj, md_table, QObject::tr("New Table") );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->setExpanded( true );
		newitem->edit();
	}
}

void
aListViewItem::newColumn()
{
	aListViewItem *newitem;
	aCfgItem newobj;

	if ( md->objClass( obj ) == md_columns )
	{
		setSelected( false );
		setExpanded( true );
		newobj = md->insert( obj, md_column, QObject::tr("New Column") );
		newitem = new aListViewItem( this, getLastChild(), md, newobj );
		newitem->edit();
	}
}

aMetadataTreeView::aMetadataTreeView(  QWidget *parent, aCfg *cfgmd )
:ananasTreeView( parent, cfgmd )
{
	aListViewItem *conf;

	if ( !md ) return;
	conf = new aListViewItem( this, md, md->find( mdc_metadata ), md->info( mda_name ));
	conf->setExpanded( true );
	connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( ContextMenu() ) );
	connect( this, SIGNAL( itemActivated( QTreeWidgetItem*, int ) ), this, SLOT( itemEdit() ) );
	connect( this, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( itemEdit() ) );
	connect( this, SIGNAL( itemCollapsed( QTreeWidgetItem* ) ), this, SLOT( on_collapsed( QTreeWidgetItem* ) ) );
}

void
aMetadataTreeView::on_collapsed( QTreeWidgetItem * item )
{
	aListViewItem *i = (aListViewItem *) item;
	if ( i )
	{
		QString oclass = md->objClass( i->obj );
		if ( oclass == md_metadata ) item->setExpanded( true );
	}
}


void
aMetadataTreeView::ContextMenu()
{

	QMenu *m=new QMenu( this );
	Q_CHECK_PTR(m);

	ContextMenuAdd(m);
	m->addAction( tr("&New"), QKeySequence(Qt::ControlModifier | Qt::Key_N), this, SLOT( itemNew() ) );
	m->exec( QCursor::pos() );
	delete m;
}


void
aMetadataTreeView::itemRename()
{
	renameItem();
}

void
aMetadataTreeView::itemNew()
{
	aListViewItem *i = (aListViewItem *) currentItem();
	if ( i )
	{
		i->newObject();
	}
}


void
aMetadataTreeView::itemDelete()
{
	deleteItem();
}


void
aMetadataTreeView::itemEdit()
{
	aListViewItem *i = (aListViewItem *) currentItem();
	if ( i )
	{
		i->edit();
	}
}

void
aMetadataTreeView::itemSave()
{
	aListViewItem *i = (aListViewItem *) currentItem();
	if ( i )
	{
		i->saveItem();
	}
}

void
aMetadataTreeView::itemLoad()
{
	aListViewItem *i = (aListViewItem *) currentItem();
	if ( i )
	{
		i->loadItem();
	}
}
void
aMetadataTreeView::itemMoveUp()
{
	moveUpItem();
}

void
aMetadataTreeView::itemMoveDown()
{

	moveDownItem();
}
