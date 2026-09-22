#include "deditaction.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include <qstatusbar.h>
#include <cstdio>
//Added by qt3to4:
#include <QPixmap>
#include <QIcon>
#include <QFileDialog>
#include <QFile>
#include "acfg.h"
#include <qtabbar.h>
//#include <qsinterpreter.h>
//#include <qseditor.h>
#include <scriptedit.h>

/*
 *  Constructs a dEditAction as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditAction::dEditAction(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);
    setObjectName(name);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditAction::~dEditAction()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditAction::languageChange()
{
    retranslateUi(this);
}

void dEditAction::setData( ActionListViewItem * o )
{
    item = o;
    md = o->md;
    obj = o->obj;
    aCfgItem active, passive;
    QString st;
    QPixmap pix, pix1;
    static const char *empty[] = {
 "1 1 1 1",
 ". c None",
 "."};
    dp = new QPixmap( empty );

    aAliasEditor *a = new aAliasEditor( md, obj, tAliases );
    al = a;
    al->setData();  // alias table init

    pActive->setPixmap( *dp ); // set empty pixmaps
    pPassive->setPixmap( *dp );
    eName->setText( md->attr( obj, mda_name ) );
    setWindowTitle( tr("Action:") + md->attr( obj, mda_name ) );
    eDescription->setPlainText( md->sText( obj, md_description ) );
    eParams->setText( md->sText( obj, md_argument ) ); // set atributes
    st = md->attr ( obj, mda_type);
    if ( st == "0" ) cbType->setCurrentIndex( 0 );
    else if ( st == "1" ) cbType->setCurrentIndex( 1 );
    eModule->setPlainText( md->sText( obj, md_sourcecode ) );
    active = md->findChild( obj, md_active_picture, 0 );
    if ( !active.isNull() ) {     // set active mode picture if exists
 bRemoveActive->setEnabled( true );
 pix.loadFromData( md->binary( active ) );
 pActive->setPixmap( pix );
 setWindowIcon( pix );
    }
    passive = md->findChild( obj, md_passive_picture, 0 );
    if ( !passive.isNull() ) {     // set passive mode picture if exists
 bRemovePassive->setEnabled( true );
 pix1.loadFromData( md->binary( passive ) );
 pPassive->setPixmap( pix1 );
    }

    QStringList tlist = md->ObjectTypes();
    otypes.clear();
    cbObject->clear();
    for ( QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it )
    {
 otypes.append( (*it).section( "\t", 0, 0 ).section(" ", 1, 1 ) );
 cbObject->addItem( (*it).section("\t", 1, 1 ));
    }
 QString oid = md->sText( obj, md_objectid );
 uint n = otypes.count();
 for ( uint i = 0; i < n; i++ )
  if ( otypes[i] == oid )
  {
   cbObject->setCurrentIndex( i );
   break;
  }
 cbObjectChange();
 n = formslist.count();
 oid = md->sText( obj, md_formid );
 for ( uint i = 0; i < n; i++ ){
  if ( formslist[i] == oid )
  {
   cbForm->setCurrentIndex( i );
   break;
  }

 }
 int type = md->sText( obj, md_actiontype ).toInt();
 switch ( type )
 {
  case 0 :
   cbAction->setCurrentIndex(md_action_new);
   break;
  case 1 :
   cbAction->setCurrentIndex(md_action_edit);
   break;
  case 2 :
   cbAction->setCurrentIndex(md_action_view);
   break;
  case 3 :
   cbAction->setCurrentIndex(md_action_execute);
   break;
 }


    cbTypeChange();
}

void dEditAction::updateMD()
{
 al->updateMD( );
 char sid[50]="";
 int id = 0;

 item->setText( 0, eName->text() );
 md->setAttr( obj, mda_name, eName->text() );
 md->setSText( obj, md_description, eDescription->toPlainText() );
 md->setSText( obj, md_argument, eParams->text() );
 if ( cbType->currentIndex() == 0 )
 {
     md->setAttr( obj, mda_type, "0" );
     id=cbAction->currentIndex();
  switch ( id )
  {
   case 0 :
   sprintf(sid, "%i", md_action_new);
   md->setSText( obj, md_actiontype, sid );
   break;
   case 1 :
   sprintf(sid, "%i", md_action_edit );
   md->setSText( obj, md_actiontype, sid );
   break;
   case 2 :
   sprintf(sid, "%i", md_action_view);
   md->setSText( obj, md_actiontype, sid );
   break;
   case 3 :
   sprintf(sid, "%i", md_action_execute);
   md->setSText( obj, md_actiontype, sid );
   break;
  }
     id=cbObject->currentIndex();
     md->setSText( obj, md_objectid, otypes[id] );
  id=cbForm->currentIndex();
  md->setSText( obj, md_formid, formslist[id] );
 }

 if ( cbType->currentIndex() == 1 )
 {
     md->setAttr( obj, mda_type, "1" );
     md->setSText( obj, md_sourcecode, eModule->toPlainText() );
 }
}


void dEditAction::init()
{

    delete statusBar();
    //eModule->setInterpreter(new QSInterpreter());
//    tabWidget2->page(1)->hide() ;
//    tabWidget2->setTabEnabled ( tabWidget2->page(1), false );
}


void dEditAction::destroy()
{
    updateMD();
    ( (MainForm*)this->window() )->wl->remove( this );
    ( (MainForm*)this->window() )->removeTab(objectName());
}


void dEditAction::cbTypeChange ()
{
//    if ( cbType->currentItem() == 0 )
// selectStandart();
 //   if ( cbType->currentItem() == 1 )
// selectUser();
// if ( cbType->currentItem() == 2 ) wStack->raiseWidget( 2 );
 wStack->setCurrentIndex( cbType->currentIndex() );
}

void dEditAction::selectStandart()
{
    lAction->show();
    lObject->show();
    lParams->show();
    cbAction->show();
    cbObject->show();
    cbForm->show();
    eParams->show();
//    tabWidget2->page(1)->hide() ;
    eModule->hide() ;
//    tabWidget2->setTabEnabled ( tabWidget2->page(1), false );
}



void dEditAction::selectUser()
{

    lAction->hide();
    lObject->hide();
    lParams->hide();
    cbAction->hide();
    cbObject->hide();
    cbForm->hide();
    eParams->hide();
    eModule->show() ;
//    tabWidget2->setTabEnabled ( tabWidget2->page(1), true );
}


void dEditAction::bSetActive_clicked()
{
    QString st;
    QPixmap pix;
//    QFileDialog *fd = new QFileDialog( "", "Images (*.png *.xpm *.jpg *.jpeg *.bmp)", 0, 0, true );
    QByteArray ba;
    QString name;
    QFile f;
//    PixmapPreview *p = new PixmapPreview;
    aCfgItem active;

//    fd->setContentsPreview( p, p );
//    fd->setPreviewMode( QFileDialog::Contents );
//    fd->setContentsPreviewEnabled( true );  // prepair file dialog
//    fd->setCaption( tr("Open image dialog" ) );
//    fd->setMode( QFileDialog::ExistingFiles );
 name= QFileDialog::getOpenFileName(this, tr("Choose an image"), QString(),
   "Images (*.png *.xpm *.jpg *.jpeg *.bmp)" );
//    if ( fd->exec() == QDialog::Accepted ) { // if accepted
// name = fd->selectedFile(); // get filename
 if ( !name.isEmpty() ) { // if not empty
     f.setFileName( name );
     if ( f.open( QIODevice::ReadOnly ) ){ // open file
  ba = f.readAll(); // read data
  f.close();  // close file
  pix.loadFromData( ba );  // load data into pixmap
  active = item->md->findChild( item->obj, md_active_picture, 0 );
  if ( active.isNull() ) active = item->md->insert( item->obj, md_active_picture, QString(), -1 );
  item->md->setBinary( active, ba ); // save data into cfg
  item->setIcon( 0, QIcon(pix) );
  setWindowIcon( pix ); // put on caption
     }
 }
// delete fd; // remove filedialog
 pActive->setPixmap( pix ); // put on form
 bRemoveActive->setEnabled( true ); // enable remove button
//    }
}


void dEditAction::bSetPassive_clicked()
{
    QString st;
    QPixmap pix;
    QByteArray ba;
    QString name;
    QFile f;
    aCfgItem passive;

    name = QFileDialog::getOpenFileName(this, tr("Open image dialog"), QString(),
        "Images (*.png *.xpm *.jpg *.jpeg *.bmp)" );
    if ( !name.isEmpty() ) {
        f.setFileName( name );
        if ( f.open( QIODevice::ReadOnly ) ){
            ba = f.readAll();
            f.close();
            pix.loadFromData( ba );
            passive = item->md->findChild( item->obj, md_passive_picture, 0 );
            if ( passive.isNull() ) passive = item->md->insert( item->obj, md_passive_picture, QString(), -1 );
            item->md->setBinary( passive, ba );
        }
        pPassive->setPixmap( pix );
        bRemovePassive->setEnabled( true );
    }
}


void dEditAction::bRemoveActive_clicked()
{
    item->md->remove( item->md->findChild( item->obj, md_active_picture, 0 ) );
    bRemoveActive->setEnabled( false );
    pActive->clear();
    item->setIcon( 0, QIcon() );
    setWindowIcon( *dp );
    pActive->setPixmap( *dp );
}


void dEditAction::bRemovePassive_clicked()
{
    item->md->remove( item->md->findChild( item->obj, md_passive_picture, 0 ) );
    bRemovePassive->setEnabled( false );
    pPassive->clear();
    pPassive->setPixmap( *dp );
}


void dEditAction::cbObjectChange()
{
    aCfgItem cobj, fobj;
 long id = otypes[cbObject->currentIndex()].toLong();
 uint n;
 cobj=md->find(id);

 cbForm->clear();
 formslist.clear();

 cbForm->addItem( QObject::tr("default") );
 formslist.append( "0" );
 if ( md->objClass( cobj ) == md_catalogue )
 {
  cbForm->addItem( QObject::tr("embedded editor") );
  formslist.append( "1" );
 }
 n = md->count( cobj, md_form );
 for ( uint i = 0; i < n; i++ )
 {
  fobj = md->find( cobj, md_form, i);
  if ( fobj.isNull() )
   continue;
  cbForm->addItem( md->attr(fobj, mda_name) );
  formslist.append(md->attr(fobj, mda_id));
 }
}
