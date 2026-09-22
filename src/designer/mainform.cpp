#include "mainform.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include "qtoolbutton.h"

#include <qapplication.h>
#include <qsettings.h>
#include "qstatusbar.h"
#include "qmessagebox.h"
#include <qapplication.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <QFrame>
#include <QIcon>
#include <QMenu>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QToolBar>
#include <QVBoxLayout>

#include "ananas.h"
//#include "adatabase.h"
//#include "acfgrc.h"
#include "cfgform.h"
//#include "messageswindow.h"
#include "dselectdb.h"
#include "alog.h"

//#include "qananastable.h"

extern CfgForm *configform;
//extern QApplication *application;
extern void messageproc(int n, const char *msg);

/*
 *  Constructs a MainForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
MainForm::MainForm(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);
    if ( name ) setObjectName( name );

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
MainForm::~MainForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MainForm::languageChange()
{
    retranslateUi(this);
}

void MainForm::filePrint()
{

}

void MainForm::fileExit()
{
//    cfg_done();
}

void MainForm::editUndo()
{

}

void MainForm::editRedo()
{

}

void MainForm::editCut()
{

}

void MainForm::editCopy()
{

}

void MainForm::editPaste()
{

}

void MainForm::editFind()
{

}

void MainForm::helpIndex()
{

}

void MainForm::helpContents()
{

}

void MainForm::helpAbout()
{
    QMessageBox::about( this, tr("About Ananas.Designer"),
			tr("<h4>Ananas.Designer %1</h4> is a programm<br>"
			   "for adjusting accounting automation system<br><br>"
			   "Copyright 2003-2006 Leader Infotech, Valery Grazdankin <br>"
			   "Copyright 2003-2006 Project Ananas, Andrey Paskal, Grigory Panov, Andrey Strelnikov<br>"
			   "License: GPL<br><br>"
			   "technical support:<br>"
			   "lider-infotech@narod.ru<br>"
			   "app@linux.ru.net<br>"
			   "Web sites:<br>"
			   "<a href=\"http://ananas.lrn.ru\">http://ananas.lrn.ru</a><br>"
			   "<a href=\"http://www.leaderit.ru\">http://www.leaderit.ru</a>"
			   "<br><br>Avalable extensions:<br>%2").arg(ananas_libversion()).arg( ""/*--AExtensionFactory::keys().join(", ")*/) );
}


void MainForm::init()
{


    setObjectName("ananas-designer_mainwindow");
    rcfile="";
    windowsMenu = new QMenu( this );
    windowsMenu->menuAction()->setCheckable( true );
    connect( windowsMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( windowsMenuAboutToShow() ) );
    menuBar()->addMenu( windowsMenu );

    menuBar()->addSeparator();
    QMenu * help = new QMenu( this );
    menuBar()->addMenu( help );

    help->addAction( tr("&About"), QKeySequence( Qt::Key_F1 ), this, SLOT(helpAbout()) );
  //  help->insertItem( tr("&Test"), this, SLOT(helpTest()));
    //    help->insertItem( "About &Qt", this, SLOT(aboutQt()));
    help->addSeparator();
    //    help->insertItem( trUtf8("Что &это"), this, SLOT(whatsThis()), SHIFT+Key_F1);


    //--designer_settings.insertSearchPath( QSettings::Unix, QString(QDir::homeDirPath())+QString("/.ananas"));
    //--designer_settings.insertSearchPath( QSettings::Windows, "/ananasgroup/ananas" );

//    QStringList lst = settings.entryList("/engine");
    QSettings designer_settings;
    designer_settings.beginGroup("/designer");
    bool maximize = designer_settings.value( "/maximize", 0 ).toBool();
    int width = designer_settings.value( "/geometry/width", 600 ).toInt();
    int height = designer_settings.value( "/geometry/height", 480 ).toInt();
    int offset_x = designer_settings.value( "/geometry/x", 0 ).toInt();
    int offset_y = designer_settings.value( "/geometry/y", 0 ).toInt();
    designer_settings.endGroup();
    resize( QSize(width, height) );//  .expandedTo(minimumSizeHint()) );
    move(offset_x,offset_y);
    if(maximize)
    {
//	   setWindowState(windowState() ^ WindowMaximized);
    }

    setWindowIcon( rcIcon("a-system.png"));
    setWindowTitle(tr("Ananas: Designer"));
    QFrame *vb = new QFrame( this );
    QVBoxLayout* vbl = new QVBoxLayout( vb );
    vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    ws = new QMdiArea( vb );
    vbl->addWidget( ws );
    wl = new aWindowsList();
    setCentralWidget( vb );
    cfgform=NULL;

    MessagesWindow *msgWindow = new MessagesWindow( this ); // , WDestructiveClose );
    addDockWidget( Qt::BottomDockWidgetArea, msgWindow );
    setMessageHandler( true );
    msgWindow->hide();
    tbMetadata->setVisible( false );
    tbActions->setVisible( false );
    tbInterface->setVisible( false );
    tbRights->setVisible( false );
    tbLanguages->setVisible( false );
    tbImageCollection->setVisible( false );
    TBToolbar->setVisible( false );
    tbTabs->setVisible( false );
    lastTabId = 0;
	lastFormId = 0;
    configSaveAction->setVisible( false );
}

void MainForm::configNew()
{
	CfgForm *cf = cfgForm();
	if (cf)
	{
		if ( !cf->close() ) return; // close configuration form if opened
    	}
    	cf = new CfgForm(ws, 0); // create new cfgform
   	if ( cf )
	{
		cf->setAttribute(Qt::WA_DeleteOnClose);
		cf->init( rcfile, true ); // initialization NEW configuration
		connectSignals( cf );
		cf->ws=ws;
		cf->show();
 		((QWidget*)cf->parent())->move(0,0);
		//--addTab(++lastTabId,cf->name());
		addTab(cf);
	}
}

void MainForm::configOpen()
{
	CfgForm *cf = cfgForm();
	if ( cf )
	{
		if ( !cf->close() ) return;
   	}
	if ( rcfile.isEmpty()) return;
	cf = new CfgForm( ws, 0);
	if ( cf )
	{
		cf->setAttribute(Qt::WA_DeleteOnClose);
		cf->init( rcfile, false );	//	initialization configuration
		connectSignals( cf );
		cf->ws=ws;
		cf->show();
 		((QWidget*)cf->parent())->move(0,0);
		//--addTab(++lastTabId,cf->name());
		addTab(cf);
    }
}

void MainForm::configSave()
{
    emit tosave();
    CfgForm *cf = cfgForm();
    if ( cf ) cf->save();	//	get configuration form and save its data
}

void MainForm::configSaveAs()
{
}



void MainForm::fileOpen()
{

}

void MainForm::fileSave()
{

}

void MainForm::fileSaveAs()
{

}

void MainForm::fileNew()
{

}

void MainForm::windowsMenuActivated( int id )
{
    QMdiSubWindow* w = ws->subWindowList().at( id );
    if ( w ) w->showNormal();
    w->setFocus();
}


void MainForm::windowsMenuAboutToShow()
{
    windowsMenu->clear();
    QAction *cascadeAction = windowsMenu->addAction(tr("&Cascade"), ws, SLOT(cascadeSubWindows() ) );
    QAction *tileAction = windowsMenu->addAction(tr("&Tile"), ws, SLOT(tileSubWindows() ) );
    QAction *horTileAction = windowsMenu->addAction(tr("Tile &horizontal"), this, SLOT(tileHorizontal() ) );
    if ( ws->subWindowList().isEmpty() ) {
	cascadeAction->setEnabled( false );
	tileAction->setEnabled( false );
	horTileAction->setEnabled( false );
    }
    windowsMenu->addSeparator();
    QList<QMdiSubWindow*> windows = ws->subWindowList();
    for ( int i = 0; i < int( windows.count() ); ++i ) {
	QAction *a = windowsMenu->addAction(windows.at(i)->windowTitle() );
	connect( a, &QAction::triggered, this, [this, i]() { windowsMenuActivated( i ); } );
	a->setCheckable( true );
	a->setChecked( ws->activeSubWindow() == windows.at(i) );
    }
}

void MainForm::tileHorizontal()
{
    // primitive horizontal tiling
    QList<QMdiSubWindow*> windows = ws->subWindowList();
    if ( !windows.count() )
	return;

    int heightForEach = ws->height() / windows.count();
    int y = 0;
    for ( int i = 0; i < int(windows.count()); ++i ) {
	QMdiSubWindow *window = windows.at(i);
	if ( window->windowState() == Qt::WindowMaximized ) {
	    // prevent flicker
	    window->hide();
	    window->showNormal();
	}
	int preferredHeight = window->minimumHeight();
	int actHeight = qMax(heightForEach, preferredHeight);

	window->setGeometry( 0, y, ws->width(), actHeight );
	y += actHeight;
    }
}


void MainForm::closeEvent( QCloseEvent *e )
{
    if (configform)
    {
	if (!configform->close())
	{
	    e->ignore();
	    return;
	}
    }

    QSettings designer_settings;
    designer_settings.beginGroup("/designer");
//	designer_settings.writeEntry( "/maximize", windowState()&WindowMaximized ? true: false);
	designer_settings.setValue( "/geometry/width", width() );
	designer_settings.setValue( "/geometry/height", height() );
	designer_settings.setValue( "/geometry/x", pos().x() );
	designer_settings.setValue( "/geometry/y", pos().y() );
	designer_settings.endGroup();
	//aLog::print(aLog::Debug,"exit");
    QMainWindow::closeEvent( e );
}


void MainForm::connectSignals( CfgForm *cf )
{
    connect(objNewFormAction, SIGNAL(activated()), cf, SLOT(newForm()));
    connect(objNewTableAction, SIGNAL(activated()), cf, SLOT(newTable()));
    connect(objNewFieldAction, SIGNAL(activated()), cf, SLOT(newField()));
    connect(objRenameAction, SIGNAL(activated()), cf->mdtree, SLOT(itemRename()));
    connect(objNewObjAction, SIGNAL(activated()), cf->mdtree, SLOT(itemNew()));
    connect(objEditAction, SIGNAL(activated()), cf->mdtree, SLOT(itemEdit()));
    connect(objDeleteAction, SIGNAL(activated()), cf->mdtree, SLOT(itemDelete()));
}


void MainForm::getMd( aCfg **md )
{
	CfgForm *cf = cfgForm();
	if ( !cf ) return;
	*md = &cf->cfg;
}

aCfg* MainForm::getMetadata()
{
	return md;
}

/*!
Get current configuration form
*/
CfgForm *
MainForm::cfgForm()
{
	CfgForm *res = 0;
	QList<QMdiSubWindow*> windows = ws->subWindowList();
	for ( int i = 0; i < int( windows.count() ); ++i ) {
		QWidget *w = windows.at(i)->widget();
		if ( strcmp( w->metaObject()->className(),"CfgForm") == 0 ){
			res = ( CfgForm *) w;
			break;
		}
	}
	return res;
}


QWidget *
MainForm::activeWindow()
{
	return ws->activeSubWindow() ? ws->activeSubWindow()->widget() : nullptr;
}

void MainForm::setId( qulonglong* id )
{
	lastFormId= *id;
}

void
MainForm::getId(qulonglong* objId)
{
	*objId = lastFormId;
}


void MainForm::addTab(int uid, const QString& winName )
{
    QList<QMdiSubWindow*> windows = ws->subWindowList();
    QToolButton* bt;

    QString S = winName;
    for ( int i = 0; i < int( windows.count() ); i++ ) {
		QWidget *w = windows.at(i)->widget();
		if( w->objectName() == S )
		{
			w->setObjectName( w->objectName() + QString("_%1").arg(uid) );
			//ixmap pixmap(*windows.at(i)->icon());
			bt = new QToolButton( tbTabs );
			bt->setText( w->windowTitle() );
			bt->setObjectName( w->objectName() );
			bt->setToolButtonStyle( Qt::ToolButtonTextOnly );
			connect( bt, SIGNAL(clicked()), w, SLOT(setFocus( )) );
			tbTabs->addWidget( bt );
//		 	bt->setAutoRaise ( true );
		//	aLog::print(aLog::Debug, QString("button name = %1").arg(bt->name()));
			break;

		}
/*		if( windows.at(i)->isShown() and !strcmp(windows.at(i)->name(),"designer_mainwindow"))
		{

			bt = new QToolButton(QIconSet(),
					     windows.at(i)->caption(),
					     "",
					     windows.at(i),
					     SLOT(setFocus( )),
					     tbTabs,
					     windows.at(i)->name() );

			bt->setUsesTextLabel ( true );
//		 	bt->setAutoRaise ( true );
			printf("button name = %s\n",bt->name());
			break;
		}
		*/
	}
   // }
//    if(windows.count()>1)
    if(tbTabs->findChildren<QToolButton*>().size() > 0)
    {
	//    tbTabs->show();

    	tbTabs->setVisible( true );
    }
    else
    {
	//   tbTabs->hide();
    	tbTabs->setVisible( false );
    }
}


void MainForm::removeTab(const  QString &winName )
{
//	int ind = winName.findRev("_");
//	printf("winName = %s\n",winName.ascii());
//	QString str = winName;
//	str = str.remove(ind,winName.length()-ind);
	QToolButton *button = tbTabs->findChild<QToolButton*>( winName );
	if(button)
	{

//		printf("deletes tab\n");
		delete button;
		button = NULL;
	}

}


void MainForm::closeChildWindows()
{

    QList<QMdiSubWindow*> windows = ws->subWindowList();
    for ( int i = 0; i < int( windows.count() ); i++ )
    {
 QWidget *w = windows.at(i)->widget();
 if(    w->metaObject()->className() != "CfgForm")
 {
  w->close();
 }
    }
}


void MainForm::fileNewAction_activated()
{

}
