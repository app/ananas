#include "deditcat.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include <qstatusbar.h>
//#include <qsinterpreter.h>
//#include <qseditor.h>
#include "acfg.h"

/*
 *  Constructs a dEditCat as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditCat::dEditCat(QWidget* parent, const char* name, Qt::WindowFlags fl)
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
dEditCat::~dEditCat()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditCat::languageChange()
{
    retranslateUi(this);
}

void dEditCat::setData( aListViewItem *o )
{
    long int i;
    int fid, fcount, fieldid, stdf;
    item = o;
    aCfg *md = o->md;
    aCfgItem obj = o->obj, sv, g, e, field;

    aAliasEditor *a = new aAliasEditor( md, obj, tAliases );
    al = a;
    al->setData();
    aRoleEditor *r = new aRoleEditor( md, obj, tRoles, md_catalogue );
    re = r;
    re->setData();
    setWindowTitle( tr("Catalogue:") + md->attr( obj, mda_name ) );
    eName->setText( md->attr( obj, mda_name ) );
    eDescription->setPlainText( md->sText( obj, md_description ) );
    g = md->find( obj, md_group ); // Find group context
    e = md->find( obj, md_element ); // Find Element context
    sv = md->find( g, md_string_view ); // Group string view
//====================================
    eStrViewFG->setPlainText( md->sText( sv, md_svfunction ) );
    eSvG->addItem( "[ standart function ]" );
    eSvG->addItem( "[ user function ]" );
    fid = md->sText( sv, md_fieldid ).toInt();
    stdf = md->attr( sv, mda_stdf ).toInt();
    eSvG->setCurrentIndex( 0 );
    for ( i = 0; i < md->count( g, md_field ); i++ ) {
	field = md->find( g, md_field, i );
	eSvG->addItem( QString("%1").arg( md->attr( field, mda_name ) ) );
	fieldid = md->id( field );
	fieldsg.insert( i, fieldid );
	if ( fid == fieldid && !stdf ) {
	    eSvG->setCurrentIndex( i + 2 );
	    eStrViewFG->setEnabled( false );
	}
    }
    if ( !fid && !stdf ) {
	eStrViewFG->setEnabled( true );
	eSvG->setCurrentIndex( 1 );
    }

//====================================
    sv = md->find( e, md_string_view );
    eStrViewF->setPlainText( md->sText( sv, md_svfunction ) );
    eSv->addItem( "[ standart function ]" );
    eSv->addItem( "[ user function ]" );
    fid = md->sText( sv, md_fieldid ).toInt();
    stdf = md->attr( sv, mda_stdf ).toInt();
    eSv->setCurrentIndex( 0 );
    for ( i = 0; i < md->count( e, md_field ); i++ ) {
	field = md->find( e, md_field, i );
	eSv->addItem( QString("%1").arg( md->attr( field, mda_name ) ) );
	fieldid = md->id( field );
	fields.insert( i, fieldid );
	if ( fid == fieldid && !stdf ) {
	    eSv->setCurrentIndex( i + 2 );
	    eStrViewF->setEnabled( false );
	}
    }
    if ( !fid && !stdf ) {
	eStrViewF->setEnabled( true );
	eSv->setCurrentIndex( 1 );
    }
}


void dEditCat::init()
{
	delete statusBar();
//	eStrViewF->setInterpreter( new QSInterpreter() );
	eStrViewF->setEnabled( false );
}


void dEditCat::updateMD()
{
    	aCfg *md = item->md;
	aCfgItem obj = item->obj, sv, g, e;

	al->updateMD();
	re->updateMD();
	item->setText( 0, eName->text().trimmed() );
	md->setAttr( obj, mda_name, eName->text().trimmed() );
	md->setSText( obj, md_description, eDescription->toPlainText() );
	g = md->find( obj, md_group ); // Find group context
	e= md->find( obj, md_element ); // Find Element context
	//======== Element view save
	sv = md->find( e, md_string_view );
	if ( sv.isNull() ) sv = md->insert( e, md_string_view );
        md->setSText( sv, md_svfunction, eStrViewF->toPlainText() );
	if ( eSv->currentIndex() == 0 ) md->setAttr( sv, mda_stdf, "1" );
	else {
	    md->setAttr( sv, mda_stdf, "0" );
	    if ( eSv->currentIndex() == 1 ) md->setSText( sv, md_fieldid, "0" );
	    else md->setSText( sv, md_fieldid, QString("%1").arg( fields.value( eSv->currentIndex() - 2 ) ) );
	}
	//======== Group view save
	sv = md->find( g, md_string_view );
	if ( sv.isNull() ) sv = md->insert( g, md_string_view );
        md->setSText( sv, md_svfunction, eStrViewFG->toPlainText() );
	if ( eSvG->currentIndex() == 0 ) md->setAttr( sv, mda_stdf, "1" );
	else {
	    md->setAttr( sv, mda_stdf, "0" );
	    if ( eSvG->currentIndex() == 1 ) md->setSText( sv, md_fieldid, "0" );
	    else md->setSText( sv, md_fieldid, QString("%1").arg( fieldsg.value( eSvG->currentIndex() - 2 ) ) );
	}

}

void dEditCat::destroy()
{
    updateMD();
    ( (MainForm*)this->window() )->wl->remove( this );
    ( (MainForm*)this->window() )->removeTab(objectName());
}




void dEditCat::eSv_activated( int index )
{
    if ( index == 1 ) eStrViewF->setEnabled( true );
    else eStrViewF->setEnabled( false );
}

void dEditCat::eSvG_activated( int index )
{
    if ( index == 1 ) eStrViewFG->setEnabled( true );
    else eStrViewFG->setEnabled( false );
}
