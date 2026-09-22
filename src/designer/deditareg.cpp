#include "deditareg.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include <qstatusbar.h>
#include "acfg.h"

/*
 *  Constructs a dEditAReg as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditAReg::dEditAReg(QWidget* parent, const char* name, Qt::WindowFlags fl)
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
dEditAReg::~dEditAReg()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditAReg::languageChange()
{
    retranslateUi(this);
}

void dEditAReg::init()
{
    delete statusBar();
}

void dEditAReg::destroy()
{
    updateMD();
    ( (MainForm*)this->window() )->wl->remove( this );
    ( (MainForm*)this->window() )->removeTab(objectName());
}

void dEditAReg::setData( aListViewItem *o )
{
	item = o;
	aCfg *md = o->md;
	aCfgItem obj = o->obj;
            aAliasEditor *a = new aAliasEditor( md, obj, tAliases );
	al = a;
	al->setData();
	setWindowTitle( tr("Accumulation register:") + md->attr( obj, mda_name ) );
	eName->setText( md->attr( obj, mda_name ) );
	eDescription->setPlainText( md->sText( obj, md_description ) );
}

void dEditAReg::updateMD()
{
	aCfg *md = item->md;
	aCfgItem obj = item->obj;

	al->updateMD();
	item->setText( 0, eName->text().trimmed() );
	md->setAttr( obj, mda_name, eName->text().trimmed());
	md->setSText( obj, md_description, eDescription->toPlainText() );
}
