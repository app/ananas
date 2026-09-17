/****************************************************************************
** $Id: atreeitems.cpp,v 1.2 2008/12/05 21:11:54 leader Exp $
**
** Header file of the Ananas visual tree object
** of Ananas Designer applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
** Copyright (C) 2003-2005 Grigory Panov <gr1313 at mail dot ru>, Yoshkar-Ola.
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

#include <qlabel.h>
#include <qcursor.h>
#include <QHeaderView>

#include "atreeitems.h"

static void moveItemAfter( QTreeWidgetItem *item, QTreeWidgetItem *after )
{
	if ( !item || !after ) return;
	QTreeWidgetItem *container = after->parent();
	if ( container ) {
		container->removeChild( item );
		container->insertChild( container->indexOfChild( after ) + 1, item );
	} else {
		QTreeWidget *t = after->treeWidget();
		if ( t ) {
			int idx = t->indexOfTopLevelItem( item );
			if ( idx >= 0 ) t->takeTopLevelItem( idx );
			t->insertTopLevelItem( t->indexOfTopLevelItem( after ) + 1, item );
		}
	}
}

ananasListViewItem::ananasListViewItem( QTreeWidget *parent, aCfg * cfgmd, aCfgItem cfgobj,
										 const QString &name )
: QTreeWidgetItem( parent )
{
	obj = cfgobj;
	md = cfgmd;
	if ( name.isNull() ) setText( 0, md->attr( obj, mda_name ) );
	else setText( 0, name );
	id = md->id(obj);
}

ananasListViewItem::ananasListViewItem( ananasListViewItem *parent, ananasListViewItem *after,
										 aCfg * cfgmd, aCfgItem cfgobj, const QString &name )
: QTreeWidgetItem( parent )
{
	moveItemAfter( this, after );
	obj = cfgobj;
	md = cfgmd;
	if ( name.isNull() ) setText( 0, md->attr( obj, mda_name ) );
	else setText( 0, name );
	id = md->id(obj);
}

ananasListViewItem::ananasListViewItem( QTreeWidget *parent, QTreeWidgetItem *after, aCfg * cfgmd, aCfgItem cfgobj, const QString &name )
: QTreeWidgetItem( parent )
{
	moveItemAfter( this, after );
	obj = cfgobj;
	md = cfgmd;
	if ( name.isNull() ) setText( 0, md->attr( obj, mda_name ) );
	else setText( 0, name );
	id = md->id(obj);
}

void
ananasListViewItem::clearTree()
{
	while ( childCount() > 0 )
		delete child( 0 );
}

ananasListViewItem *
ananasListViewItem::previousSibling()
{
	if ( !this ) return 0;
	QTreeWidgetItem *p = parent();
	if ( p ) {
		int idx = p->indexOfChild( this );
		if ( idx > 0 ) return (ananasListViewItem *) p->child( idx - 1 );
		return 0;
	}
	QTreeWidget *t = treeWidget();
	if ( t ) {
		int idx = t->indexOfTopLevelItem( this );
		if ( idx > 0 ) return (ananasListViewItem *) t->topLevelItem( idx - 1 );
	}
	return 0;
}

ananasListViewItem*
ananasListViewItem::nextSibling()
{
	QTreeWidgetItem *p = parent();
	if ( p ) {
		int idx = p->indexOfChild( this );
		if ( idx >= 0 ) return (ananasListViewItem *) p->child( idx + 1 );
		return 0;
	}
	QTreeWidget *t = treeWidget();
	if ( t ) {
		int idx = t->indexOfTopLevelItem( this );
		if ( idx >= 0 ) return (ananasListViewItem *) t->topLevelItem( idx + 1 );
	}
	return 0;
}

ananasListViewItem*
ananasListViewItem::getLastChild()
{
	if ( childCount() == 0 ) return 0;
	return (ananasListViewItem*) child( childCount() - 1 );
}


ananasTreeView::ananasTreeView ( QWidget *parent, aCfg *cfgmd )
:QTreeWidget ( parent )
{
	md = cfgmd;
	setColumnCount( 1 );
	header()->hide();
	setSortingEnabled( false );
	setSelectionMode( QAbstractItemView::SingleSelection );
	setContextMenuPolicy( Qt::CustomContextMenu );
};


void
ananasTreeView::ContextMenuAdd( QMenu * m )
{
	Q_UNUSED(m);
};
