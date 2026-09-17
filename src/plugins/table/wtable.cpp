/****************************************************************************
** $Id: wtable.cpp,v 1.1 2008/11/05 21:16:30 leader Exp $
**
** Code file of the table plugin of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
** Copyright (C) 2006 Grigory Panov <gr1313 at mail.ru>, Yoshkar-Ola.
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

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <qlayout.h>
#include <qstring.h>
#include <qvariant.h>
#include <qobject.h>

#include "acfg.h"
#include "aobject.h"
#include "awidget.h"
#include "wtable.h"


wTable::wTable(QWidget *parent, const char * name):QTableWidget(parent)
{
	setObjectName( name );
}

wTable::~wTable()
{
}


void
wTable::setDocument(int row, aObject *object)
{
	Q_UNUSED(row);
	Q_UNUSED(object);
}

void
wTable::setText(int row, int col, const QString& text)
{
	QTableWidgetItem *it = item( row, col );
	if ( !it ) {
		it = new QTableWidgetItem();
		setItem( row, col, it );
	}
	it->setText( text );
}


QString
wTable::text(int col, int row)
{
	QTableWidgetItem *it = item( row, col );
	return it ? it->text() : QString();
}

void
wTable::setHeaderText(int col, const QString& text)
{
	QTableWidgetItem *h = horizontalHeaderItem( col );
	if ( !h ) {
		h = new QTableWidgetItem();
		setHorizontalHeaderItem( col, h );
	}
	h->setText( text );
}


QString
wTable::textHeader(int col)
{
	QTableWidgetItem *h = horizontalHeaderItem( col );
	return h ? h->text() : QString();
}

void
wTable::sortColumn ( int col, bool ascending, bool wholeRows )
{
	Q_UNUSED(wholeRows);
	sortItems( col, ascending ? Qt::AscendingOrder : Qt::DescendingOrder );
}


void
wTable::setColumnWidth(int col, int width)
{
	QTableWidget::setColumnWidth(col,width);
}


int
wTable::columnWidth(int col)
{
	return QTableWidget::columnWidth(col);
}

void
wTable::setNumCols ( int r )
{
	setColumnCount( r );
}

int
wTable::numCols () const
{
	return columnCount();
}
void
wTable::hideColumn ( int col )
{
	setColumnHidden( col, true );
}

void
wTable::showColumn ( int col )
{
	setColumnHidden( col, false );
}

void
wTable::adjustColumn ( int col )
{
	resizeColumnToContents( col );
}

void
wTable::setColumnStretchable ( int col, bool stretch )
{
	horizontalHeader()->setSectionResizeMode( col, stretch ? QHeaderView::Stretch : QHeaderView::Interactive );
}

bool
wTable::isColumnStretchable ( int col ) const
{
	return horizontalHeader()->sectionResizeMode( col ) == QHeaderView::Stretch;
}


void
wTable::swapColumns ( int col1, int col2, bool swapHeader )
{
	if ( col1 == col2 ) return;
	for ( int row = 0; row < rowCount(); ++row ) {
		QTableWidgetItem *a = takeItem( row, col1 );
		QTableWidgetItem *b = takeItem( row, col2 );
		if ( b ) setItem( row, col1, b );
		if ( a ) setItem( row, col2, a );
	}
	if ( swapHeader ) {
		QTableWidgetItem *ha = takeHorizontalHeaderItem( col1 );
		QTableWidgetItem *hb = takeHorizontalHeaderItem( col2 );
		if ( hb ) setHorizontalHeaderItem( col1, hb );
		if ( ha ) setHorizontalHeaderItem( col2, ha );
	}
}

void
wTable::swapCells ( int row1, int col1, int row2, int col2 )
{
	if ( row1 == row2 && col1 == col2 ) return;
	QTableWidgetItem *a = takeItem( row1, col1 );
	QTableWidgetItem *b = takeItem( row2, col2 );
	if ( b ) setItem( row1, col1, b );
	if ( a ) setItem( row2, col2, a );
}

void
wTable::setCurrentCell ( int row, int col )
{
	QTableWidget::setCurrentCell( row, col );
}

void
wTable::setColumnReadOnly ( int col, bool ro )
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
wTable::insertColumns ( int col, int count )
{
	for ( int i = 0; i < count; ++i )
		insertColumn( col );
}

void
wTable::removeColumn ( int col )
{
	QTableWidget::removeColumn( col );
}

void
wTable::editCell ( int row, int col, bool replace )
{
	Q_UNUSED(replace);
	editItem( item( row, col ) );
}

void
wTable::columnClicked ( int col )
{
	Q_UNUSED(col);
}

void
wTable::swapRows ( int row1, int row2, bool swapHeader )
{
	if ( row1 == row2 ) return;
	for ( int col = 0; col < columnCount(); ++col ) {
		QTableWidgetItem *a = takeItem( row1, col );
		QTableWidgetItem *b = takeItem( row2, col );
		if ( b ) setItem( row1, col, b );
		if ( a ) setItem( row2, col, a );
	}
	if ( swapHeader ) {
		QTableWidgetItem *ha = takeVerticalHeaderItem( row1 );
		QTableWidgetItem *hb = takeVerticalHeaderItem( row2 );
		if ( hb ) setVerticalHeaderItem( row1, hb );
		if ( ha ) setVerticalHeaderItem( row2, ha );
	}
}
