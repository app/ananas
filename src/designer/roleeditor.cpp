/****************************************************************************
** $Id: roleeditor.cpp,v 1.1 2008/11/05 21:16:27 leader Exp $
**
** Code file of the Alias editor of Ananas
** Designer applications
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

#include "roleeditor.h"
#include "acfg.h"


aRoleEditor::aRoleEditor( aCfg *c, aCfgItem o, QTableWidget *t, const char *p )
{
    ac = c;
    obj = o;
    tRoles = t;
    tRoles->setRowCount( 0 );
    tRoles->setColumnCount( 1 );
    tRoles->setHorizontalHeaderItem( 0, new QTableWidgetItem( tr("Read") ) );
    parent = p;
    if ( !strcmp( parent, md_document ) ) {
	tRoles->setColumnCount( 5 );
	tRoles->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Write") ) );
	tRoles->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Delete") ) );
	tRoles->setHorizontalHeaderItem( 3, new QTableWidgetItem( tr("Turn On") ) );
	tRoles->setHorizontalHeaderItem( 4, new QTableWidgetItem( tr("Turn Off") ) );
    }
    if ( !strcmp( parent, md_catalogue ) ) {
	tRoles->setColumnCount( 3 );
	tRoles->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("Write") ) );
	tRoles->setHorizontalHeaderItem( 2, new QTableWidgetItem( tr("Delete") ) );
    }

}

aRoleEditor::~aRoleEditor()
{
}

void
aRoleEditor::setData()
{
    int i, j, n;
    aCfgItem orole, roles, role;
    QString rolename, right;

    roles = ac->find( ac->find( mdc_root ), md_roles, 0 );
    roleCount = ac->count( roles, md_role );
    n = ac->countChild( obj, md_role );
    tRoles->setRowCount( roleCount );
    for ( i = 0; i < roleCount; i++ ) {
	role = ac->findChild( roles, md_role, i );
	rolename = ac->attr( role, mda_name );
	tRoles->setVerticalHeaderItem( i, new QTableWidgetItem( rolename ) );
	QTableWidgetItem *r = new QTableWidgetItem( QString() );
	tRoles->setItem( i, 0, r );
	QTableWidgetItem *w = 0;
	QTableWidgetItem *d = 0;
	QTableWidgetItem *on = 0;
	QTableWidgetItem *off = 0;
	if ( !strcmp( parent, md_catalogue ) ) {
		w = new QTableWidgetItem( QString() );
		d = new QTableWidgetItem( QString() );
		tRoles->setItem( i, 1, w );
		tRoles->setItem( i, 2, d );
	    }
	if ( !strcmp( parent, md_document ) ) {
		w = new QTableWidgetItem( QString() );
		d = new QTableWidgetItem( QString() );
		on = new QTableWidgetItem( QString() );
		off = new QTableWidgetItem( QString() );
		tRoles->setItem( i, 1, w );
		tRoles->setItem( i, 2, d );
		tRoles->setItem( i, 3, on );
		tRoles->setItem( i, 4, off );
	    }
	for ( j = 0; j < n; j++) {
	    orole = ac->findChild( obj, md_role, j );
	    if ( rolename == ac->attr( orole, mda_name ) ) {
		right = ac->attr( orole, mda_rights );
		if ( right.indexOf( "-r" ) > -1 ) r->setCheckState( Qt::Checked );
		if ( w && right.indexOf( "-w" ) > -1 ) w->setCheckState( Qt::Checked );
		if ( d && right.indexOf( "-d" ) > -1 ) d->setCheckState( Qt::Checked );
		if ( on && right.indexOf( "-on" ) > -1 ) on->setCheckState( Qt::Checked );
		if ( off && right.indexOf( "-off" ) > -1 ) off->setCheckState( Qt::Checked );
	    }
	}
    }
}

void aRoleEditor::updateMD()
{
    int i;
    aCfgItem role;

    do {
	role = ac->findChild( obj, md_role, 0 ) ;
	if ( !role.isNull() ) ac->remove( role );
    } while ( !role.isNull() );
    for ( i = 0; i < tRoles->rowCount(); i++ ) {
	QString right;
	QTableWidgetItem *q;
	role = ac->insert( obj, md_role, tRoles->item( i, 0 )->text(), -1 );
	QTableWidgetItem *h = tRoles->verticalHeaderItem( i );
	ac->setAttr( role, mda_name, h ? h->text() : QString() );
	q = (QTableWidgetItem *)tRoles->item( i, 4 );
	if ( q ) if ( q->checkState() == Qt::Checked ) right.insert( 0, "-off" );
	q = (QTableWidgetItem *)tRoles->item( i, 3 );
	if ( q ) if ( q->checkState() == Qt::Checked ) right.insert( 0, "-on" );
	q = (QTableWidgetItem *)tRoles->item( i, 2 );
	if ( q ) if ( q->checkState() == Qt::Checked ) right.insert( 0, "-d" );
	q = (QTableWidgetItem *)tRoles->item( i, 1 );
	if ( q ) if ( q->checkState() == Qt::Checked ) right.insert( 0, "-w" );
	q = (QTableWidgetItem *)tRoles->item( i, 0 );
	if ( q ) if ( q->checkState() == Qt::Checked ) right.insert( 0, "-r" );
	ac->setAttr( role, mda_rights, right );
    }
}

