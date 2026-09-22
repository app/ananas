/****************************************************************************
** $Id: atreeitems.cpp,v 1.2 2008/11/08 20:16:35 leader Exp $
**
** Header file of the Ananas visual tree object
** of Ananas Designer applications
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

#include <QHeaderView>
#include <QLabel>
#include <QCursor>

#include "atreeitems.h"
#include "alog.h"

// Move item to be right after `after` (QTreeWidgetItem::moveItem equivalent).
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

void
ananasListViewItem::moveUp ()
{
	if(!previousSibling()) return; // not previous item! - no changes
	aCfgItem item = previousSibling()->obj;

	if ( obj.isNull() )
	{
		aLog::print(aLog::Error, QObject::tr(" Ananas List View Item %1 is null").arg(md->attr(obj,mda_name)));
		return;
	}
	if( md->swap( obj, item ) )
	{
		moveItemAfter( previousSibling(), this );
		aLog::print(aLog::Debug, QObject::tr("Ananas List View Item swaping"));
	}
	else
	{
		aLog::print(aLog::Error, QObject::tr("Ananas List View Item swaping"));
	}
}

void
ananasListViewItem::moveDown ()
{
	if(!nextSibling()) return; // not next item! - no changes
	aCfgItem item = nextSibling()->obj;
	if ( item.isNull() )
	{
		aLog::print(aLog::Error, QObject::tr(" Ananas List View Item %1 is null").arg(md->attr(obj,mda_name)));
		return;
	}
	if( md->swap( obj, item ) )
	{
		moveItemAfter( this, nextSibling() );
		aLog::print(aLog::Debug, QObject::tr("Ananas List View Item swaping"));
	}
	else
	{
		aLog::print(aLog::Error, QObject::tr("Ananas List View Item swaping"));
	}
}

ananasListViewItem *
ananasListViewItem::previousSibling()
{
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
		if ( idx >= 0 && idx + 1 < p->childCount() ) return (ananasListViewItem *) p->child( idx + 1 );
		return 0;
	}
	QTreeWidget *t = treeWidget();
	if ( t ) {
		int idx = t->indexOfTopLevelItem( this );
		if ( idx >= 0 && idx + 1 < t->topLevelItemCount() ) return (ananasListViewItem *) t->topLevelItem( idx + 1 );
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
	connect( this, &QTreeWidget::itemChanged, this, &ananasTreeView::itemRenamed );
}


void
ananasTreeView::ContextMenuAdd( QMenu * m )
{
	m->addAction( tr("&Rename"), QKeySequence(Qt::ControlModifier | Qt::Key_R), this, SLOT( itemRename() ) );
	m->addAction( tr("&Edit"),  QKeySequence(Qt::ControlModifier | Qt::Key_E), this, SLOT( itemEdit() ) );
	m->addAction( tr("&Delete"), QKeySequence(Qt::ControlModifier | Qt::Key_D), this, SLOT( itemDelete() ) );
	m->addAction( tr("&MoveUp"), QKeySequence(Qt::ControlModifier | Qt::Key_U), this, SLOT( itemMoveUp() ) );
	m->addAction( tr("&MoveDown"), QKeySequence(Qt::ControlModifier | Qt::Key_M), this, SLOT( itemMoveDown() ) );
	m->addAction( tr("&SaveItem"), this, SLOT( itemSave() ) );
	m->addAction( tr("&LoadItem"), this, SLOT( itemLoad() ) );
	m->addSeparator();
}

void
ananasTreeView::deleteItem()
{
	ananasListViewItem *i = (ananasListViewItem *) currentItem();
	if ( i )
	{
		if ( i->id ) {
			md->remove( i->obj );
			delete i;
		}
	}
}

void
ananasTreeView::moveUpItem()
{
	ananasListViewItem *i = (ananasListViewItem *) currentItem();
	if ( i )
	{
		if ( i->id ) {
			i->moveUp();
		}
	}
}

void
ananasTreeView::moveDownItem()
{
	ananasListViewItem *i = (ananasListViewItem *) currentItem();
	if ( i )
	{
		if ( i->id ) {
			i->moveDown();
		}
	}
}

void
ananasTreeView::renameItem()
{
	QTreeWidgetItem *i = currentItem();
	if ( i ) editItem( i, 0 );
}

void
ananasTreeView::itemRenamed( QTreeWidgetItem *item, int column )
{
	ananasListViewItem *i = (ananasListViewItem *) item;
	if ( !i || column != 0 || i->obj.isNull() || !i->md ) return;
	const QString name = item->text( 0 ).trimmed();
	if ( name != i->md->attr( i->obj, mda_name ) )
		i->md->setAttr( i->obj, mda_name, name );
}
