/****************************************************************************
** $Id: atoolbar.cpp,v 1.1 2008/11/05 21:16:29 leader Exp $
**
** Code file of the Ananas Tool bar of Ananas
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

//#include <qimage.h>
#include "atoolbar.h"
//Added by qt3to4:
#include <QPixmap>

aToolBar::aToolBar( aCfg *cfg, aCfgItem &obj, aEngine *e, QMainWindow* parent , const char* name )
: QToolBar( parent )
{
	setObjectName( name );
	md = cfg;
	en = e;
	ReadTool( obj );
}

aToolBar::~aToolBar(){
}

void
aToolBar::ReadTool( aCfgItem &obj )
{
    aCfgItem aobj, apix;	// action and pixmap XML data
    QString aKey;	// key sequence
    long pid;	// action id

    aobj = md->firstChild( obj );	//from first child action
    while ( !aobj.isNull() ) {		// foreach not null
	aKey = md->sText ( aobj, md_key );	//key sequence
	pid = md->id( aobj );	// action id
	apix = md->findChild(
		md->find(
			md->text( md->findChild( aobj, md_comaction, 0 ) ).toLong() ),
		            md_active_picture,
                  		0
		);	// first action pixmap cfg object
	QPixmap pix( md->binary( apix ) );	// pixmap
	QAction *a = new QAction(
		QIcon(pix), // pixmap
		md->attr( aobj, mda_name), // name
		this // owner
	);	// create new action
	if ( !aKey.isEmpty() ) a->setShortcut( QKeySequence( aKey ) );
	actions.insert( pid, a );	// add action to dict
	addAction( a );	// put action into toolbar
	connect( a, SIGNAL(triggered()), this, SLOT(on_Item()) );	// connect to slot
	aobj = md->nextSibling( aobj );	// get next action
    }
}

void
aToolBar::on_Item()
{
    QHashIterator<int, QAction*> it( actions );	//dict iterator
    while ( it.hasNext() ) {	// foreach action
	it.next();
	if ( it.value() == sender() ) {	// sender object
	    en->on_MenuBar( it.key() );	// call slot
	    break;	// break cycle
	}
    }
}
