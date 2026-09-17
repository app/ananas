/****************************************************************************
** $Id: mainform.h,v 1.1 2008/11/05 21:16:26 leader Exp $
**
** Header file of the Main form of Ananas Engine application
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
**
** This file is part of the Engine application of the Ananas
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

/****************************************************************************
****************************************************************************/

#ifndef ANANASMAINFORM_H
#define ANANASMAINFORM_H
#include <qvariant.h>
#include <QMdiArea>
//#include <qsinterpreter.h>
#include <qdialog.h>
#include <qstringlist.h>
#include <QMainWindow>
#include <qapplication.h>
#include <qsettings.h>
//Added by qt3to4:
#include <QPixmap>
#include <QMenu>
#include "ananas.h"
#include "amenubar.h"
#include "atoolbar.h"
#include "awindowslist.h"
#include "engine.h"


class QAction;
class QToolBar;
class QMenu;

/*!
 * \ru
 * Главное окно приложения Ананас, запускаемого пользователем.
 *
 * \_ru
 */

class MainForm : public QMainWindow {
	Q_OBJECT

public:
	MainForm( QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::Window );
	~MainForm();
	aEngine engine;
	aCfg *md;

	AMenuBar* menubar;
	QMenu *windowsMenu;
	QMdiArea* ws;
	aWindowsList* wl;
	QString rcfile;

public slots:
	bool init();
	bool initEngine();
	void initMenuBar();
	void initStatusBar();
	void initActionBar();
	void InsertMainMenu(QString text, QMenu *pop);
	void Exit(int code);
	void close();
	void helpAbout();
	void statusMessage( const QString &msg );
	void windowsMenuAboutToShow();
	void windowsMenuActivated();
	void tileHorizontal();
	void setBackground( const QPixmap &pix );

protected:
	protected slots:
	virtual void languageChange();

private:
	QSettings engine_settings;
};

extern MainForm *mainform;
extern QMdiArea *mainformws;
extern aWindowsList *mainformwl;

#endif // ANANASMAINFORM_H
