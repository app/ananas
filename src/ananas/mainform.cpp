/****************************************************************************
** $Id: mainform.cpp,v 1.2 2008/12/06 22:56:39 leader Exp $
**
** Code file of the Main form of Ananas Engine application
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
#include <qmenubar.h>
#include <qmessagebox.h>
#include <QToolBar>
#include <qstatusbar.h>
#include <qworkspace.h>
#include <qapplication.h>
#include <qaction.h>
#include <qdir.h>
//Added by qt3to4:
#include <QFrame>
#include <QPixmap>
#include <QMenu>
#include <QVBoxLayout>
#include <QBrush>

#include "mainform.h"

#include "ananas.h"

MainForm *mainform=NULL;
QWorkspace *mainformws=NULL;
aWindowsList *mainformwl=NULL;
//extern void messageproc(int n, const char *msg);

/*
 *  Constructs a AnanasMainWindow as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
MainForm::MainForm( QWidget* parent, const char* name, Qt::WFlags fl )
    : QMainWindow( parent, fl )
{
//    QApopupmenu *popup;
    QFrame	*vb = new QFrame(this);
    QVBoxLayout	*vbl = new QVBoxLayout( vb );

    setWindowIcon( rcIcon("a-system.png"));
    vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    ws = new QWorkspace( vb );
    vbl->addWidget( ws );
    wl = new aWindowsList();
    ws->setScrollBarsEnabled( TRUE );
    setCentralWidget( vb );
    statusBar()->setObjectName("statusbar");
    if ( name ) setObjectName( name );
    else setObjectName( "mainwindow" );
    //--engine_settings.insertSearchPath( QSettings::Unix, QString(QDir::homePath())+QString("/.ananas"));
    //--engine_settings.insertSearchPath( QSettings::Windows, "/ananasgroup/ananas" );

//    QStringList lst = settings.entryList("/engine");
    engine_settings.beginGroup("/engine");
    bool maximize = engine_settings.value( "maximize", 0 ).toBool();
    int width = engine_settings.value( "geometry/width", 600 ).toInt();
    int height = engine_settings.value( "geometry/height", 480 ).toInt();
    int offset_x = engine_settings.value( "geometry/x", 0 ).toInt();
    int offset_y = engine_settings.value( "geometry/y", 0 ).toInt();
    engine_settings.endGroup();
    resize( QSize(width, height) );//  .expandedTo(minimumSizeHint()) );
    move(offset_x,offset_y);
    if(maximize)
    {
//	   setWindowState(windowState() ^ WindowMaximized);
    }
    rcfile="";
}

bool
MainForm::init()
{
    MessagesWindow *msgWindow = new MessagesWindow( this );// , WDestructiveClose );
    addDockWidget( Qt::BottomDockWidgetArea, msgWindow );
    setMessageHandler( true );
    msgWindow->hide();
    if ( !initEngine() ) return false;
    connect( menubar, SIGNAL(activated(int)), &engine, SLOT(on_MenuBar(int)) );
    languageChange();
//    engine.project.addObject(menubar);
//    engine.project.addObject(this);
//    engine.project.addObject(statusBar());
    engine.code->newQObject(menubar);
    engine.code->newQObject(this);
    engine.code->newQObject(statusBar());

    languageChange();
    engine.on_systemstart();
    return true;
}

bool
MainForm::initEngine()
{
    if (rcfile.isEmpty()) return false;
    if ( !engine.init( rcfile ) ) return false;
    engine.ws = ws;
    engine.wl = wl;
    connect( &engine, SIGNAL( statusMessage( const QString & ) ), this, SLOT( statusMessage( const QString & ) ) );
    md = engine.md;
    if ( !md ) return false;
    initMenuBar();
    initStatusBar();
    initActionBar();
    return true;
}

void
MainForm::initMenuBar()
{
	QMenu *m;
	m = new QMenu( this );
	windowsMenu = new QMenu( this );
    	connect( windowsMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( windowsMenuAboutToShow() ) );
	m->addAction(rcIcon("a-system.png"), tr( "About" ), this, SLOT( helpAbout() ));
	//windowsMenu->addAction(rcIcon("a-system.png"), tr( "Windows" ), this, SLOT( windowsMenuAboutToShow() ));
        menubar = new AMenuBar( md, this, "menubar");
	InsertMainMenu( tr("&Help"), m );
    	InsertMainMenu( tr("&Windows"), windowsMenu );
	menuBar()->show();
}

void
MainForm::initStatusBar()
{
}

void
MainForm::initActionBar()
{
    aCfgItem obj, item;

    obj = md->find( md->find( mdc_interface ), md_toolbars );	// look for md_toolbars
    item = md->firstChild( obj );
    while ( !item.isNull() ) {	// foreach md_toolbar
	new aToolBar( md, item, &engine, this, md->attr( item, mda_name ).toLatin1().constData() );		// create toolbar
	item = md->nextSibling( item );	// next md_toolbar
    }
}

void
MainForm::helpAbout()
{
    QMessageBox::about( this, tr("About Ananas.Engine program"),
			tr("<h4>Ananas.Engin %1</h4> is a programm "
			   "for executing application of accounting automation system<br><br>"
			   "Copyright 2003-2008 Leader Infotech, Valery Grazdankin, "
			   "Copyright 2003-2008 Project Ananas, Andrey Paskal, Grigory Panov, Andrey Strelnikov<br>"
			   "License: GPL<br><br>"
			   "technical support:<br>"
			   "lider-infotech@narod.ru<br>"
			   "app@linux.ru.net<br>"
			   "Web sites:<br>"
			   "<a href=\"http://ananas.lrn.ru\">http://ananas.lrn.ru</a><br>"
			   "<a href=\"http://www.leaderit.ru\">http://www.leaderit.ru</a>"
			   "<br>Avalable extensions:<h4>%2</h4><br>"
			   ).arg(ananas_libversion()).arg( AExtensionFactory::keys().join(", ") ) );
}

void MainForm::InsertMainMenu(QString text, QMenu *pop){
    menubar->insertItem(text, pop);

}


void
MainForm::Exit(int code)
{

	qApp->exit(code);
}
void
MainForm::close()
{
        engine_settings.beginGroup("/engine");
//	engine_settings.writeEntry( "/maximize", windowState()&WindowMaximized ? true: false);
	engine_settings.setValue( "geometry/width", width() );
	engine_settings.setValue( "geometry/height", height() );
	engine_settings.setValue( "geometry/x", pos().x() );
	engine_settings.setValue( "geometry/y", pos().y() );
	engine_settings.endGroup();
	printf("exit\n" );
//MainWindow::close();
}

void
MainForm::statusMessage( const QString &msg )
{
 	statusBar()->showMessage( msg );
}

void
MainForm::setBackground( const QPixmap &pix ){
	ws->setBackground( QBrush(pix) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MainForm::~MainForm()
{
	engine.on_systemstop();
	close();
//	printf("Mainform destructor\n");
	if(ws) delete ws;
	ws = 0;
	if(wl) delete wl;
	wl=0;
	if(menubar) delete menubar;
	menubar=0;
//	printf("delete mainform\n");
//    cfg->on_systemstop();
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MainForm::languageChange()
{
    setWindowTitle(QString( tr("Ananas")+" "+ananas_libversion() )+": "+md->info( md_info_name ) );
//    setWindowTitle( tr( "Ananas VERSION" ) );
}

void MainForm::windowsMenuAboutToShow()
{
    windowsMenu->clear();
    QAction *cascadeAction = windowsMenu->addAction(tr("&Cascade"), ws, SLOT(cascade() ) );
    QAction *tileAction = windowsMenu->addAction(tr("&Tile"), ws, SLOT(tile() ) );
    QAction *horTileAction = windowsMenu->addAction(tr("Tile &horizontal"), this, SLOT(tileHorizontal() ) );
    if ( ws->windowList().isEmpty() ) {
	cascadeAction->setEnabled( false );
	tileAction->setEnabled( false );
	horTileAction->setEnabled( false );
    }
    windowsMenu->addSeparator();
    QWidgetList windows = ws->windowList();
    if(windows.count()==0) return;
    int i=0, count = windows.count();

    do
    {
	if(windows.at(i) && windows.at(i)->isHidden())
	{
		++i;
		continue;
	}
	QAction *a = windowsMenu->addAction(windows.at(i)->windowTitle() );
	a->setData( i );
	a->setCheckable( true );
	a->setChecked( ws->activeWindow() == windows.at(i) );
	connect( a, SIGNAL( triggered() ), this, SLOT( windowsMenuActivated() ) );
	++i;
    }while( i < count );
}

void MainForm::tileHorizontal()
{
    // primitive horizontal tiling
    QWidgetList windows = ws->windowList();
    int count =0;
    for ( int i = 0; i < int(windows.count()); ++i )
    {
	QWidget *window = windows.at(i);
	if( !window ) continue;
	if(!window->isHidden()) count++;
    }
    if ( !count ) return;

    int heightForEach = ws->height() / count;
    int y = 0;
    for ( int i = 0; i < windows.count(); ++i )
    {
	QWidget *window = windows.at(i);
	if ( !window ) continue;
	if ( window->isHidden() ) continue;
	if ( window->windowState() == Qt::WindowMaximized )
	{
	    // prevent flicker
	    window->hide();
	    window->showNormal();
	}
	int preferredHeight = window->minimumHeight()+window->parentWidget()->baseSize().height();
	int actHeight = qMax(heightForEach, preferredHeight);

	window->parentWidget()->setGeometry( 0, y, ws->width(), actHeight );
	y += actHeight;
    }
}

void MainForm::windowsMenuActivated()
{
    QAction *a = qobject_cast<QAction*>( sender() );
    if ( !a ) return;
    int id = a->data().toInt();
    QWidget* w = ws->windowList().at( id );
    if ( w ) { w->showNormal(); w->setFocus(); }
}
