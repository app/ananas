/****************************************************************************
** $Id: amenubar.cpp,v 1.1 2008/11/05 21:16:28 leader Exp $
**
** Code file of the Ananas Menu bar of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
**
** This file is part of the Library of the Ananas
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

/******************************************************************
 ******************************************************************/

#include "amenubar.h"
#include <QPixmap>


AMenuBar::AMenuBar( QWidget* parent , const char* name  )
	:QMenuBar( parent ) {
	setObjectName( name );
}

AMenuBar::AMenuBar( aCfg *cfg, QWidget* parent , const char* name  )
:QMenuBar( parent )
{
	setObjectName( name );
	md = cfg;
	ReadMenu( md->find( md->find( mdc_interface ), md_mainmenu) );
}

void
AMenuBar::ReadMenu( aCfgItem obj )
{
	aCfgItem	cobj;
	QString		text, aKey ;
	long 		id;

	if ( !md )
		return;
	cobj = md->firstChild ( obj );
	while ( !cobj.isNull() )
	{
		id = md->id( cobj );
		if ( md->objClass ( cobj ) == md_submenu )
		{
			QMenu *menu = addMenu( md->attr( cobj, mda_name ) );
			ReadMenu( menu, cobj );
		}
		if ( md->objClass ( cobj ) == md_command )
		{
			text = md->sText ( cobj, md_menutext );
			if ( text == "" ) text = md->attr( cobj, mda_name );
			aKey = md->sText ( cobj, md_key );
			QAction *a = addAction( text );
			a->setData( (int) id );
			if ( !aKey.isEmpty() ) a->setShortcut( QKeySequence( aKey ) );
			connect( a, SIGNAL( triggered() ), this, SLOT( on_Item() ) );
		}
		if ( md->objClass ( cobj ) == md_separator )
		{
			addSeparator();
		}
		cobj = md->nextSibling ( cobj );
	}

}

void
AMenuBar::ReadMenu( QMenu *parent, aCfgItem obj )
{
	aCfgItem	cobj, apix;
	QString		text, aKey;
	long id, pid;
    QPixmap		pix;

	if ( !md )
		return;

	cobj = md->firstChild ( obj );
	while ( !cobj.isNull() )
	{
		id = md->id( cobj );
		if ( md->objClass ( cobj ) == md_submenu )
		{
			QMenu *menu = parent->addMenu( md->attr ( cobj, mda_name ) );
			ReadMenu( menu, cobj );
		}
		if ( md->objClass ( cobj ) == md_command )
		{
			text = md->sText ( cobj, md_menutext );
			if ( text == "" )
				text = md->attr( cobj, mda_name );
			aKey = md->sText ( cobj, md_key );
			pid = md->text( md->findChild( cobj, md_comaction, 0 ) ).toLong();
			apix = md->findChild( md->find( pid ), md_active_picture, 0 );
			pix.loadFromData( md->binary( apix ) );
			QAction *a = parent->addAction( text );
			if ( !pix.isNull() ) a->setIcon( QIcon( pix ) );
			a->setData( (int) id );
			if ( !aKey.isEmpty() ) a->setShortcut( QKeySequence( aKey ) );
			connect( a, SIGNAL( triggered() ), this, SLOT( on_Item() ) );
			pix = QPixmap();
		}
		if ( md->objClass ( cobj ) == md_separator )
		{
			parent->addSeparator();
		}
		cobj = md->nextSibling ( cobj );
	}

}


AMenuBar::~AMenuBar(){
}

QMenu *AMenuBar::insertItem ( const QString & text, QMenu * popup ) {
	addMenu( popup );
	popup->setTitle( text );
	return popup;
}

void AMenuBar::on_Item() {
	QAction *a = qobject_cast<QAction*>( sender() );
	if ( a ) emit activated( a->data().toInt() );
}
