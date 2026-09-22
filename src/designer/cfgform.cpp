#include "cfgform.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include <qlabel.h>
#include <qpixmap.h>
#include <qstatusbar.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qinputdialog.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QIcon>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMdiSubWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
//#include <iostream.h>

#include "ananas.h"
#include "mainform.h"
#include "deditfield.h"
#include "deditcfg.h"
#include "deditrole.h"
#include "deditreport.h"
#include "deditdialog.h"
#include "deditlang.h"
#include "mdtree.h"
#include "actiontree.h"
#include "alog.h"

/*
 *  Constructs a CfgForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
CfgForm::CfgForm(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);
    if ( name ) setObjectName( name );

    (void)statusBar();
}

/*
 *  Destroys the object and frees any allocated resources
 */
CfgForm::~CfgForm()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CfgForm::languageChange()
{
    retranslateUi(this);
}

extern MainForm *mainform;

void
set_Icon(QTreeWidgetItem *item, const char *name)
{
    char in[200];
    strcpy(in, name);
    strcat(in,".png");
    item->setIcon(0, QIcon(rcIcon(in)));
}


/*class ObjEditing {
    public:
    ObjEditing(){item=NULL; widget=NULL;}
    ObjEditing(QListViewItem *aitem, QWidget *awidget) {item=aitem;widget=awidget;}

    QListViewItem *item;
    QWidget *widget;
};
*/
//typedef QValueList<ObjEditing> ObjEditingList;
//ObjEditingList ObjEdList;

CfgForm *configform=NULL;


void CfgForm::fileNew()
{

}

void CfgForm::fileOpen()
{

}

void CfgForm::fileSave()
{

}

void CfgForm::fileSaveAs()
{

}

void CfgForm::filePrint()
{

}

void CfgForm::fileExit()
{
}


void
CfgForm::init( QString &rcf, bool flagNew )	//	flagNew shows if it needs to create new configuration
{
    statusBar()->hide();
    configform=this;
    rcfile = rcf;	//	resource file
    setWindowIcon( rcIcon("a-system.png"));
    if ( !flagNew ) {	// read rcfile
	if ( cfg.readrc( rcfile ) ) {
		aLog::print(aLog::Error,tr("CfgForm readrc error"));
		return;
	}
    }
//    globalCfg = &cfg;
    setWindowTitle(tr("Business scheme: ")+cfg.info( mda_name ));
//	QWidget *tab_0 = new QWidget( tabWidget, "tab" );
    QGridLayout *l = new QGridLayout( tabWidget->widget(0) );
    l->setContentsMargins(1, 1, 1, 1);
    l->setSpacing(2);
    mdtree = new aMetadataTreeView( tabWidget->widget(0), &cfg );	// metadata tree creation
    l->addWidget( mdtree, 0, 0);
//	tabWidget->insertTab( tab_0, tr( "Business scheme" ), 0 );
//	tabWidget->setCurrentPage( 0 );
    initLang();	//	languages tab initialization
    initRoles();	//	rights tab initialization
    initImageCollection();	//	image collection tab initialization
    mainform->TBToolbar->setVisible( true );	//	show edit toolbar
    mainform->configSaveAction->setVisible( true );	//	show save button

    actiontree = new aActionTreeView ( tabWidget->widget(1), &cfg );	//	action tree creation
    QGridLayout *j = new QGridLayout( tabWidget->widget(1) );
    j->setContentsMargins(1, 1, 1, 1);
    j->setSpacing(2);
    j->addWidget( actiontree, 0, 0);

    interfacetree = new InterfaceTreeView ( tabWidget->widget(2), &cfg );	//	interface tree creation
    QGridLayout *k = new QGridLayout( tabWidget->widget(2) );
    k->setContentsMargins(1, 1, 1, 1);
    k->setSpacing(2);
    k->addWidget( interfacetree, 0, 0);
    // connection toolbar actions
    connect(mainform->objMetadataNewAction, SIGNAL(activated()), mdtree, SLOT( itemNew() ) );
    connect(mainform->objMetadataEditAction, SIGNAL(activated()), mdtree, SLOT( itemEdit() ) );
    connect(mainform->objActionsNewAction, SIGNAL(activated()), actiontree, SLOT( itemNewAction() ) );
    connect(mainform->objActionsNewGroupAction, SIGNAL(activated()), actiontree, SLOT( itemNewGroup() ) );
    connect(mainform->objActionsEditAction, SIGNAL(activated()), actiontree, SLOT( itemEdit() ) );
    connect(mainform->objInterfaceNewCommandAction, SIGNAL(activated()), interfacetree, SLOT( itemNewCommand() ) );
    connect(mainform->objInterfaceNewSubmenuAction, SIGNAL(activated()), interfacetree, SLOT( itemNewSubmenu() ) );
    connect(mainform->objInterfaceNewSeparatorAction, SIGNAL(activated()), interfacetree, SLOT( itemNewSeparator() ) );
    connect(mainform->objInterfaceEditAction, SIGNAL(activated()), interfacetree, SLOT( itemEdit() ) );
    connect(mainform->objImageCollectionAddAction, SIGNAL(activated()), this, SLOT( bAddImage_clicked() ) );
    connect(mainform->objLanguagesNewAction, SIGNAL(activated()), this, SLOT( bAdd_clicked() ) );
    connect(mainform->objLanguagesEditAction, SIGNAL(activated()), this, SLOT( bEdit_clicked() ) );
    connect(mainform->objLanguagesClearAction, SIGNAL(activated()), this, SLOT( bKill_clicked() ) );
    connect(mainform->objRoleNewAction, SIGNAL(activated()), this, SLOT( bAddRole_clicked() ) );
    connect(mainform->objRoleEditAction, SIGNAL(activated()), this, SLOT( bEditRole_clicked() ) );
    connect(mainform->objRoleClearAction, SIGNAL(activated()), this, SLOT( bKillRole_clicked() ) );
}

/*
  * Image collection initialization
  */
void CfgForm::initImageCollection()
{
    aCfgItem oroot, image_collection, image;
    int i, n;
    QPixmap pix;
    QString name;

    oroot = cfg.find( mdc_root );	// get root
    image_collection = cfg.find( oroot, md_image_collection, 0 );	// get image col part
    if ( image_collection.isNull() ) cfg.insert( oroot, md_image_collection, QString(), -1 );
    n = cfg.countChild( image_collection, md_image );	// count images
    for ( i = 0; i < n; i++ ) {
	image = cfg.findChild( image_collection, md_image, i );	// foreach
	name = cfg.attr( image, mda_name );	// get name
	pix.loadFromData( cfg.binary( image ) );	// load binary
	QListWidgetItem *item = new QListWidgetItem( QIcon(pix), name, vImageCollection );	// create image
	item->setFlags( item->flags() | Qt::ItemIsEditable );	// set rename enabled
	vImageCollection->insertItem( 0, item );	// insert image
	idList.insert( cfg.id( image ), item );		// add image into dict
    }
    if ( vImageCollection->count() ) bRemoveImage->setEnabled( true );	// enable remove button
}

/*
  * Add image into collection
  */
void CfgForm::bAddImage_clicked()
{
    QPixmap pix;
    QFileDialog *fd = new QFileDialog( 0, tr("Open image dialog"), QString(), "Images (*.png *.xpm *.jpg *.jpeg *.bmp)" );
    QByteArray ba;
    QString name;
    QStringList names;
    aCfgItem image_collection, image;
    QFile f;

    fd->setFileMode( QFileDialog::ExistingFiles );		// tune file dialogue
    if ( fd->exec() == QDialog::Accepted ) {		// if accepted
	names = fd->selectedFiles();			// get selected filenames
	QStringList::Iterator it = names.begin();
	while( it != names.end() ) {			// trough the names
	    name = *it;
	    ++it;
	    if ( !name.isEmpty() ) {			// if not empty
		f.setFileName( name );
		if ( f.open( QIODevice::ReadOnly ) ){		// open r\o mode
				ba = f.readAll();	// get binary
				f.close();
				pix.loadFromData( ba );	// load pixmap from binary
			} else continue;
		}
		QFileInfo fi( name );
		name = fi.baseName();
		QListWidgetItem *item = new QListWidgetItem( QIcon(pix), name, vImageCollection );
		item->setFlags( item->flags() | Qt::ItemIsEditable );		// set rename enabled
		vImageCollection->insertItem( 0, item );	// insert image
		image_collection = cfg.findChild( cfg.find( mdc_root ), md_image_collection, 0 );
		image = cfg.insert( image_collection, md_image, name );	// insert image metaitem
		cfg.setBinary( image, ba );		// set binary
		idList.insert( cfg.id( image ), item );	// add image into dict
	    }
	delete fd;					// delete file dialogue
	bRemoveImage->setEnabled( true );		// remove button enable
	mainform->objTBDeleteAction->setEnabled( true );	// toolbar button enable
    }
}

/*
  * Remove image from collection
  */
void CfgForm::bRemoveImage_clicked()
{
    QListWidgetItem *current;
    aCfgItem image_collection, image;
    int i, n, id;

    image_collection = cfg.findChild( cfg.find( mdc_root ), md_image_collection, 0 );
    n = cfg.countChild( image_collection, md_image );		// count image in collection
    current = vImageCollection->currentItem();			// get current selected image
    for ( i = 0; i < n; i++ ) {
	image = cfg.findChild( image_collection, md_image, i );	// get metaitem
	id = cfg.id( image );					// get id
	if ( current == idList.value( id ) ) {			// if corresponds
	    cfg.remove( image );
	    idList.remove( id );				// rmove from metadata and from dict
	    delete current;
	    break;
	}
    }
    if ( !vImageCollection->count() ) {				// if last image removed
	bRemoveImage->setEnabled( false );			// disable remove button and toolbar button
	mainform->objTBDeleteAction->setEnabled( false );
    }
    vImageCollection->sortItems( Qt::AscendingOrder );	// turn image sorting on
}

/*
  * Image rename event handler
  */
void CfgForm::vImageCollection_itemRenamed( QListWidgetItem *item, const QString &name )
{
    aCfgItem image_collection, image;
    int i, n;

    image_collection = cfg.findChild( cfg.find( mdc_root ), md_image_collection, 0 );
    n = cfg.countChild( image_collection, md_image );		// count images
    for ( i = 0; i < n; i++ ) {
	image = cfg.findChild( image_collection, md_image, i );	// get timage
	if ( item == idList.value( cfg.id( image ) ) ) {		// if corresponds
	    cfg.setAttr( image, mda_name, name );		// rename metaitem
	    break;
	}
    }
}

/*
  * Languages initialization
  */
void CfgForm::initLang()
{
	aCfgItem oroot;
	aCfgItem lang, obj;
	int i, n;

	tLang->setRowCount( 0 );
	oroot = cfg.find( mdc_root );		// get root
	if ( oroot.isNull() ) {
		aLog::print(aLog::Info,tr("CfgForm metadata root is null"));
	}
	lang = cfg.findChild( oroot, md_languages, 0 );	// get lang part
	if ( lang.isNull() ) {
		aLog::print(aLog::Info,tr("CfgForm metadata lang is null"));
	}
	if ( lang.isNull() ) lang = cfg.insert( oroot, md_languages, QString(), -1 );
	n = cfg.count( lang, md_language );		// count languages
	aLog::print(aLog::Debug,QString(tr("lang = %1")).arg(n));
	for ( i = 0; i < n; i++ ) {			// foreach
		obj = cfg.find( lang, md_language, i);		// get lang
		if ( obj.isNull() ) aLog::print(aLog::Debug,QString(tr("CfgForm %1 metadata lang is null")).arg(i));
		tLang->insertRow( tLang->rowCount() );	// insert into table
		if ( !tLang->item( tLang->rowCount()-1, 0 ) ) tLang->setItem( tLang->rowCount()-1, 0, new QTableWidgetItem() );
		tLang->item( tLang->rowCount()-1, 0 )->setText( cfg.attr( obj, mda_tag ) );
		if ( !tLang->item( tLang->rowCount()-1, 1 ) ) tLang->setItem( tLang->rowCount()-1, 1, new QTableWidgetItem() );
		tLang->item( tLang->rowCount()-1, 1 )->setText( cfg.attr( obj, mda_name ) );
		if ( !tLang->item( tLang->rowCount()-1, 2 ) ) tLang->setItem( tLang->rowCount()-1, 2, new QTableWidgetItem() );
		tLang->item( tLang->rowCount()-1, 2 )->setText( cfg.attr( obj, mda_trfile ) );	// fill cols
	}
	if ( !tLang->rowCount() ) {	//	turn buttons off
		bDelete->setEnabled( false );
		bEdit->setEnabled( false );
		bKill->setEnabled( false);
		mainform->objLanguagesClearAction->setEnabled( false );
		mainform->objLanguagesEditAction->setEnabled( false );
	}
}

/*
  * Roles initialization
  */
void CfgForm::initRoles()
{
    aCfgItem oroot;
    aCfgItem role, obj;
    int i, n;
    QString rd;

    tRole->setRowCount( 0 );				// no rows
    oroot = cfg.find( mdc_root );			// get root
    role = cfg.find( oroot, md_roles, 0 );		// get role part
    if ( role.isNull() ) role = cfg.insert( oroot, md_roles, QString(), -1 );
    n = cfg.count( role, md_role );			// count roles
    for ( i = 0; i < n; i++ ) {
	obj = cfg.find( role, md_role, i);		// get role
	tRole->insertRow( tRole->rowCount() );	// insert this
	if ( !tRole->item( tRole->rowCount()-1, 0 ) ) tRole->setItem( tRole->rowCount()-1, 0, new QTableWidgetItem() );
	tRole->item( tRole->rowCount()-1, 0 )->setText( cfg.attr( obj, mda_name ) );
	rd = cfg.sText( obj, md_description );
	rd.replace( QChar('\n'), " " );			// remove \n symbol for good perception
	if ( !tRole->item( tRole->rowCount()-1, 1 ) ) tRole->setItem( tRole->rowCount()-1, 1, new QTableWidgetItem() );
	tRole->item( tRole->rowCount()-1, 1 )->setText( rd ); // fill cols
    }
    if ( !tRole->rowCount() ) {	//	turn unneccessary buttons off
	bDeleteRole->setEnabled( false );
	bEditRole->setEnabled( false );
	bKillRole->setEnabled( false);
	mainform->objRoleClearAction->setEnabled( false );
	mainform->objRoleEditAction->setEnabled( false );
    }
}

/*
  * Destroy configuration form
  */
void CfgForm::destroy()
{
	configform = 0;
//	globalCfg = 0;			// clear pointers
	mainform->cfgform = NULL;
        mainform->removeTab(objectName());
}

/*
  * Save metadata on disk
  */
void CfgForm::save()
{
//    int i, n;
//    aCfgItem oroot, lang, obj;
    aDatabase *database = aDatabase::database();

    if ( cfg.writerc( rcfile ) ) cfg_message(0, tr("Unable to write rc file.").toUtf8().constData());
    else {
//    	if ( database.init( rcfile ) ){
		database->update();
//		database.done();
//	}
  }
}

/*
  * Add new language
  */
void CfgForm::bAdd_clicked()
{
    aCfgItem lang, obj;

    tLang->insertRow( tLang->rowCount() );			// insert new row
    tLang->setCurrentCell( tLang->rowCount()-1, 0 );		// focus on new role
    lang = cfg.find( cfg.find( mdc_root ), md_languages, 0 );	// get lang part
    obj = cfg.insert( lang, md_language, tr("New language") );	// insert new metaitem
    dEditLang *e = new dEditLang( ws, 0 );
    e->setAttribute(Qt::WA_DeleteOnClose);
    e->setData( configform, obj );
    e->show();						// create editor, set its data and show this one
    bDelete->setEnabled( true );
    bEdit->setEnabled( true );
    bKill->setEnabled( true );				// enable control buttons and toolbar actions
    mainform->objLanguagesClearAction->setEnabled( true );
    mainform->objLanguagesEditAction->setEnabled( true );
    mainform->objTBDeleteAction->setEnabled( true );
}

/*
  * Delete language
  */
void CfgForm::bDelete_clicked()
{
   QTableWidgetItem *curLangName = tLang->item( tLang->currentRow(), 1 );
   if ( QMessageBox::warning( this, tr("Deleting row"), tr("Delete '%1'?").arg( curLangName ? curLangName->text() : QString() ), QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes ) {
// request for delete
       aCfgItem lang, obj, alias;
       int i, n;

       lang = cfg.find( cfg.find( mdc_root ), md_languages, 0 );	// get lang part
       n = cfg.count( lang, md_language );	// count langs
       for ( i = 0; i < n; i++ ) {
	obj = cfg.find( lang, md_language, i );		// foreach lang
	QTableWidgetItem *curLangTag = tLang->item( tLang->currentRow(), 0 );
	if ( cfg.attr( obj, mda_tag ) == ( curLangTag ? curLangTag->text() : QString() ) ) {
	    cfg.remove( obj );			// if tags corresponding remove metaitem
	    break;
	}
       }
       tLang->removeRow( tLang->currentRow() );		// remove row
       if ( !tLang->rowCount() ) {
	   bDelete->setEnabled( false );
	   bEdit->setEnabled( false );
	   bKill->setEnabled( false );		// if last row removed then buttons and actions are disabled
	   mainform->objLanguagesClearAction->setEnabled( false );
	   mainform->objLanguagesEditAction->setEnabled( false );
	   mainform->objTBDeleteAction->setEnabled( false );
       }
   }
}

/*
  * Language editing
  */
void CfgForm::bEdit_clicked()
{
    aCfgItem obj;

    obj = cfg.find( cfg.find( cfg.find( mdc_root ), md_languages, 0 ), md_language, tLang->currentRow() );
    // get lang metaitem
    dEditLang *e = new dEditLang( ws, 0 );
    e->setAttribute(Qt::WA_DeleteOnClose);
    e->setData( configform, obj );
    e->show();				// create editor, set its data and show this one
}

/*
  * Double-click table event
  */
void CfgForm::tLang_doubleClicked( int, int, int, const QPoint & )
{
    bEdit_clicked();		// call edit button click event
}

/*
  * Kill dead aliases througout metadata
  */
void CfgForm::bKill_clicked()
{
    aCfgItem oroot, alias;
    int i = 0, s, n, ac, killed = 0;

    oroot = cfg.find( mdc_root );		// get root
    n = cfg.count( oroot, md_alias );		// count aliases
    while ( i < n ) {				// foreach
	ac = 0;					// unset counter
	alias = cfg.find( oroot, md_alias, i );	// get alias
	for ( s = 0; s < tLang->rowCount(); s++ ) // check aliase tag on existing in lang list
	    if ( cfg.attr( alias, mda_tag ) == ( tLang->item( s, 0 ) ? tLang->item( s, 0 )->text() : QString() ) ) ac++;
	if ( !ac ) {
	    cfg.remove( alias );
	    killed++;
	    n--;				// if dead then delete this and change counters
	} else i++;
    }
    QMessageBox::warning( this, tr("Clearing dead aliases"), tr("%1 dead aliases killed.").arg( killed ), tr("OK") );
    // information message box
}

void CfgForm::newObj()
{
}


void CfgForm::listCfg_onItem( QTreeWidgetItem * )
{

}



void CfgForm::newField()
{
}



void CfgForm::newForm()
{
}


void CfgForm::newTable()
{
}

/*
  * Cofiguration form close event
  */
void CfgForm::closeEvent( QCloseEvent *e )
{

	if ( !cfg.modified() ) {		// if data didn't modify send event further
	QMainWindow::closeEvent( e );
	return;
    }
    switch( QMessageBox::warning( this, tr("Saving changes"),
				  tr("Save changes %1?").arg( windowTitle() ),
				  tr("Yes"), tr("No"), tr("Cancel") ) ) {
	// request for saving before exit
    case 0:	// yes
	{
		mainform->closeChildWindows();
		save();			// save data and send event further
		QMainWindow::closeEvent( e );
/*
	    cancelupdate=0;
	    if (closeSubWindows()) e->ignore();
	    else {
		if (mainform) mainform->configSave();
		QMainWindow::closeEvent( e );
	    }
*/
	}
	break;
    case 1:	// no
//	if (closeSubWindows()) {//e->ignore();
//	}
//	else {
////	    cancelupdate=1;
//	    //e->accept();
//	}				// send event further
		mainform->closeChildWindows();
        	QMainWindow::closeEvent(e);
	break;
    default:
	e->ignore();			// ignore event by default
	break;
	mainform->TBToolbar->setVisible( false );
	mainform->configSaveAction->setVisible( false );
	mainform->tbMetadata->setVisible( false );
	mainform->tbActions->setVisible( false );
	mainform->tbInterface->setVisible( false );		// disable toolbars anyway
	mainform->tbRights->setVisible( false );
	mainform->tbLanguages->setVisible( false );
	mainform->tbImageCollection->setVisible( false );
//	closeSubWindows();
    }
    //    } else {
    //        e->accept();
    //    }
}

/*
  * Subwindow close event
  */
int
CfgForm::closeSubWindows()
{
    QList<QMdiSubWindow*> windows = ws->subWindowList();		// workspace windows list
    if ( windows.count() ) {				// if there're opened windows
	for ( int i = 0; i < int(windows.count()); ++i ) {	// foreach
	    QWidget *window = windows.at( i )->widget();
	    if (window!=this) {				// if not this
	    	if ( QString( window->metaObject()->className() ) == tr ( "messageswindow" ) ) {
			delete window;	// if messageswindow remove it
			continue;
		}
		if ( !window->close() ) return 1;	// if cannot close return error
		else delete window;			// else delete window
	    }
	}
    }
    return 0;
}



/*
int CfgForm::activateExist( QListViewItem *item )
{
    QWidget *window;

    window=findOpened(item);
    if (window) {
	window->setFocus();
	return 1;
    }
    return 0;
}
*/

//QWidget *
//CfgForm::findOpened( QListViewItem *item )
//{
/*
    ObjEditingList::iterator it;
    QWidgetList windows = ws->windowList();
    QWidget *window, *res=NULL;
    int f;

    if ( windows.count() ) {
	it = ObjEdList.begin();
	while ( it != ObjEdList.end()) {
	    f=0;
	    for ( int i = 0; i < int(windows.count()); ++i ) {
		window = windows.at( i );
		if (window==(*it).widget) {
		    f=1; break;
		}
	    }
	    if (!f) {
		it=ObjEdList.remove(it);
	    } else it++;
	}
	for ( it = ObjEdList.begin(); it != ObjEdList.end(); ++it ) {
	    if (item==(*it).item) {
		res=(*it).widget;
		break;
	    }
	}
    }
    return res;
*/
//    return NULL;
//}

/*
  * When tab switches some actions reconnect to corresponding toolbar buttons and toolbars recombinate
  */
void CfgForm::tabWidget_selected( const QString &tab )
{
    bool fM, fA, fI, fR, fL, fIC;

    if ( tab == tr("Metadata") ) {
	connect(mainform->objTBDeleteAction, SIGNAL(activated()), mdtree, SLOT( itemDelete() ) );
	connect(mainform->objTBRenameAction, SIGNAL(activated()), mdtree, SLOT( itemRename() ) );
	connect(mainform->objTBMoveUpAction, SIGNAL(activated()), mdtree, SLOT( itemMoveUp() ) );
	connect(mainform->objTBMoveDownAction, SIGNAL(activated()), mdtree, SLOT( itemMoveDown() ) );
	fM = true;
    } else {
	    disconnect(mainform->objTBDeleteAction, SIGNAL(activated()), mdtree, SLOT( itemDelete() ) );
	    disconnect(mainform->objTBRenameAction, SIGNAL(activated()), mdtree, SLOT( itemRename() ) );
	    disconnect(mainform->objTBMoveUpAction, SIGNAL(activated()), mdtree, SLOT( itemMoveUp() ) );
	    disconnect(mainform->objTBMoveDownAction, SIGNAL(activated()), mdtree, SLOT( itemMoveDown() ) );
	    fM= false;
	}
    if ( tab == tr("Actions") ) {
	connect(mainform->objTBDeleteAction, SIGNAL(activated()), actiontree, SLOT( itemDelete() ) );
	connect(mainform->objTBRenameAction, SIGNAL(activated()), actiontree, SLOT( itemRename() ) );
	connect(mainform->objTBMoveUpAction, SIGNAL(activated()), actiontree, SLOT( itemMoveUp() ) );
	connect(mainform->objTBMoveDownAction, SIGNAL(activated()), actiontree, SLOT( itemMoveDown() ) );
	fA = true;
    } else {
	    disconnect(mainform->objTBDeleteAction, SIGNAL(activated()), actiontree, SLOT( itemDelete() ) );
	    disconnect(mainform->objTBRenameAction, SIGNAL(activated()), actiontree, SLOT( itemRename() ) );
	    disconnect(mainform->objTBMoveUpAction, SIGNAL(activated()), actiontree, SLOT( itemMoveUp() ) );
	    disconnect(mainform->objTBMoveDownAction, SIGNAL(activated()), actiontree, SLOT( itemMoveDown() ) );
	    fA = false;
	}
    if ( tab == tr("Interface") ) {
	connect(mainform->objTBDeleteAction, SIGNAL(activated()), interfacetree, SLOT( itemDelete() ) );
	connect(mainform->objTBRenameAction, SIGNAL(activated()), interfacetree, SLOT( itemRename() ) );
	connect(mainform->objTBMoveUpAction, SIGNAL(activated()), interfacetree, SLOT( itemMoveUp() ) );
	connect(mainform->objTBMoveDownAction, SIGNAL(activated()), interfacetree, SLOT( itemMoveDown() ) );
	fI = true;
    } else {
	    disconnect(mainform->objTBDeleteAction, SIGNAL(activated()), interfacetree, SLOT( itemDelete() ) );
	    disconnect(mainform->objTBRenameAction, SIGNAL(activated()), interfacetree, SLOT( itemRename() ) );
	    disconnect(mainform->objTBMoveUpAction, SIGNAL(activated()), interfacetree, SLOT( itemMoveUp() ) );
	    disconnect(mainform->objTBMoveDownAction, SIGNAL(activated()), interfacetree, SLOT( itemMoveDown() ) );
	    fI = false;
	}
    if ( tab == tr("Rights") )
    {
	 connect(mainform->objTBDeleteAction, SIGNAL(activated()), this, SLOT( bDeleteRole_clicked() ) );
	 fR = true;
	 if ( !tRole->rowCount() ) mainform->objTBDeleteAction->setEnabled( false );
	 else mainform->objTBDeleteAction->setEnabled( true );
     } else {
	 disconnect(mainform->objTBDeleteAction, SIGNAL(activated()), this, SLOT( bDeleteRole_clicked() ) );
	 fR = false;
     }
     if ( tab == tr("Languages") ) {
	 connect(mainform->objTBDeleteAction, SIGNAL(activated()), this, SLOT( bDelete_clicked() ) );
	 fL = true;
	 if ( !tLang->rowCount() ) mainform->objTBDeleteAction->setEnabled( false );
	 else mainform->objTBDeleteAction->setEnabled( true );
     } else {
	 disconnect(mainform->objTBDeleteAction, SIGNAL(activated()), this, SLOT( bDelete_clicked() ) );
	 fL = false;
     }
     if ( tab == tr("Image collection") ) {
	 connect(mainform->objTBDeleteAction, SIGNAL(activated()), this, SLOT( bRemoveImage_clicked() ) );
	 fIC = true;
	 if ( !vImageCollection->count() ) mainform->objTBDeleteAction->setEnabled( false );
	 else mainform->objTBDeleteAction->setEnabled( true );
     } else {
	 disconnect(mainform->objTBDeleteAction, SIGNAL(activated()), this, SLOT( bRemoveImage_clicked() ) );
	 fIC = false;
     }
     if ( tab == tr("Image collection") || tab == tr("Languages") || tab == tr("Rights") ) {
	 mainform->objTBRenameAction->setVisible( false );
	 mainform->objTBMoveUpAction->setVisible( false );
	 mainform->objTBMoveDownAction->setVisible( false );
     } else {
	 mainform->objTBRenameAction->setVisible( true );
	 mainform->objTBMoveUpAction->setVisible( true );
	 mainform->objTBMoveDownAction->setVisible( true );
	 mainform->objTBDeleteAction->setEnabled( true );
     }
     mainform->tbMetadata->setVisible( fM );
     mainform->tbActions->setVisible( fA );
     mainform->tbInterface->setVisible( fI );
     mainform->tbRights->setVisible( fR );
     mainform->tbLanguages->setVisible( fL );
     mainform->tbImageCollection->setVisible( fIC );
}

/*
  * Adds new role
  */
void CfgForm::bAddRole_clicked()
{
    aCfgItem role, obj, roles;

    tRole->insertRow( tRole->rowCount() );	// insert row into table
    tRole->setCurrentCell( tRole->rowCount()-1, 0 );	// focus on new cell
    roles = cfg.find( cfg.find( mdc_root ), md_roles, 0 ); // get role part of configuration
    obj = cfg.insert( roles, md_role, tr("New role") );	// insert new role into part
    dEditRole *e = new dEditRole( ws, 0 );
    e->setAttribute(Qt::WA_DeleteOnClose);
    e->setData( configform, obj );
    e->show();	// create role editor, set its data and show this one
    bDeleteRole->setEnabled( true );
    bEditRole->setEnabled( true );
    bKillRole->setEnabled( true );
    mainform->objRoleClearAction->setEnabled( true );
    mainform->objRoleEditAction->setEnabled( true );
    mainform->objTBDeleteAction->setEnabled( true ); // enable buttons and actions
}

/*
  * Delete role
  */
void CfgForm::bDeleteRole_clicked()
{
    QTableWidgetItem *curRoleName = tRole->item( tRole->currentRow(), 0 );
    if ( QMessageBox::warning( this, tr("Deleting row"), tr("Delete '%1'?").arg( curRoleName ? curRoleName->text() : QString() ), QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes ) {
// request for deleting
	aCfgItem roles, obj, role;
	int i, n;

	roles = cfg.find( cfg.find( mdc_root ), md_roles, 0 );	// get role part of configuration
	n = cfg.count( roles, md_role );	// count roles
	for ( i = 0; i < n; i++ ) {	// foreach role
	    obj = cfg.find( roles, md_role, i );	// get this one
	    QTableWidgetItem *roleName = tRole->item( tRole->currentRow(), 0 );
	    if ( cfg.attr( obj, mda_name ) == ( roleName ? roleName->text() : QString() ) ) {
		cfg.remove( obj );
		break;			// if found delete and break the cycle
	    }
	}
	tRole->removeRow( tRole->currentRow() );	// remove row from table
	if ( !tRole->rowCount() ) {		// if there're no rows disable buttons and actions
	    bDeleteRole->setEnabled( false );
	    bEditRole->setEnabled( false );
	    bKillRole->setEnabled( false );
	    mainform->objRoleClearAction->setEnabled( false );
	    mainform->objRoleEditAction->setEnabled( false );
	    mainform->objTBDeleteAction->setEnabled( false );
	}
    }
}

/*
  * Edit curren role
  */
void CfgForm::bEditRole_clicked()
{
    int objid;
    aCfgItem obj;

    obj = cfg.find( cfg.find( cfg.find( mdc_root ), md_roles, 0 ), md_role, tRole->currentRow() );
    // get role
/*    printf( "name = %s\n", cfg.attr( obj, mda_name ).ascii() );
    objid = cfg.id( obj );
    if ( wl->find( objid ) ) {
	wl->get( objid )->setFocus();
	return;
    }
*/
    dEditRole *e = new dEditRole( ws, 0 );
    e->setAttribute(Qt::WA_DeleteOnClose);
    e->setData( configform, obj );
    e->show();		// create role editor, set its data and show this one
}

/*
  * Kill dead roles throughout the configuration
  */
void CfgForm::bKillRole_clicked()
{
    aCfgItem oroot, roles, role;
    int i = 0, s, n, ac, killed = 0;

    oroot = cfg.find( mdc_root );	// get configuration root
    n = cfg.count( oroot, md_role );	// count roles throughout configuration
    while ( i < n ) {	// foreach role
	ac = 0;		// unset counter
	role = cfg.find( oroot, md_role, i );	// get this one
	for ( s = 0; s < tRole->rowCount(); s++ ) // look through role table for correspondence
	    if ( cfg.attr( role, mda_name ) == ( tRole->item( s, 0 ) ? tRole->item( s, 0 )->text() : QString() ) ) ac++; // inc counter
	if ( !ac ) {
	    cfg.remove( role );
	    killed++;
	    n--;	// if counter then remove role, inc kill-counter and dec main counter
	} else i++;	// else inc current role number
    }
    QMessageBox::warning( this, tr("Clearing dead roles"), tr("%1 dead roles killed.").arg( killed ), tr("OK") );
    // display count of killed roles
}


void CfgForm::tRole_doubleClicked( int, int, int, const QPoint & )
{
    bEditRole_clicked();	// call role edit method
}

