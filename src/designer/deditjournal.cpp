#include "deditjournal.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include <qstatusbar.h>
#include "acfg.h"

/*
 *  Constructs a dEditJournal as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditJournal::dEditJournal(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    Q_UNUSED(name);
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditJournal::~dEditJournal()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditJournal::languageChange()
{
    retranslateUi(this);
}

void dEditJournal::setData( aListViewItem *o )
{
    item = o;
    md = o->md;
    obj = o->obj;
    aCfgItem cobj, doc, docs;
    int i, j, n;
    aAliasEditor *a = new aAliasEditor( md, obj, tAliases );
    al = a;
    al->setData();
    aRoleEditor *r = new aRoleEditor( md, obj, tRoles, md_journal );
    re = r;
    re->setData();
    setWindowTitle( tr("Journal:") + md->attr( obj, mda_name ) );
    eName->setText( md->attr( obj, mda_name ) );
    eDescription->setPlainText( md->sText( obj, md_description ) );

    i = md->attr ( obj, mda_type ).toInt();
    cbType-> setCurrentIndex(i);

    journalDocs = new ananasTreeView( tabWidget18->widget(2),  md  );
    journalDocs->setSortingEnabled( false );
    // TODO Fix me!!!
    //--layout28->addWidget( journalDocs, 0, 0 );
    n = md->count( obj, md_used_doc );
    for ( j = 0; j < n; j++ )
    {
	cobj = md->find( obj, md_used_doc, j );
	if ( cobj.isNull() )
	    continue;
//		did = md->text(cobj).toLong();
	doc = md->find(md->text(cobj).toLong());
	if ( doc.isNull() )
	    md->remove(cobj);
	else
	    new ananasListViewItem( journalDocs, journalDocs->topLevelItem( journalDocs->topLevelItemCount() - 1 ), md, doc );
    }

    allDocs = new ananasTreeView( tabWidget18->widget(2),  md  );
    allDocs->setSortingEnabled( false );
    // TODO Fix me!!!
    //--layout29->add( allDocs );
    docs = md->find(md->find(mdc_metadata),md_documents,0);
    n = md->count( docs, md_document );
    for ( j = 0; j < n; j++ )
    {
	cobj = md->find( docs, md_document, j );
	if ( cobj.isNull() )
	    return;
	//		did = md->id(cobj);
	doc = md->find(md->id(cobj));
	new ananasListViewItem( allDocs, allDocs->topLevelItem( allDocs->topLevelItemCount() - 1 ), md, doc );
    }
    docPage = tabWidget18->widget(2);
    if ( cbType->currentIndex() != 1 )
	typeChange();
}


void dEditJournal::init()
{
    statusBar()->hide();
}


void dEditJournal::updateMD()
{
    aCfgItem doc, docs;
    int i;

    al->updateMD();
    re->updateMD();
    item->setText( 0, eName->text().trimmed() );
    md->setAttr( obj, mda_name, eName->text().trimmed() );
    md->setSText( obj, md_description, eDescription->toPlainText() );
    md->setAttr( obj, mda_type, cbType->currentIndex() );
    docs = md->find(obj,md_columns,0);
    do
    {
	doc = md->findChild( docs, md_used_doc, 0 );
	if ( !doc.isNull() )
	    md->remove( doc );
    } while ( !doc.isNull() );
    if ( cbType->currentIndex() != 1 )
	return;
    ananasListViewItem *aitem = (ananasListViewItem *)journalDocs->topLevelItem(0);
    for ( i = 0; i < journalDocs->topLevelItemCount(); i++ )
    {
	doc = md->insert( docs, md_used_doc, QString(), -1 );
	md->setText( doc, QString( "%1" ).arg( aitem->id ) );
	aitem = (ananasListViewItem *)aitem->nextSibling();
    }

}

void dEditJournal::destroy()
{
    updateMD();
    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    ( (MainForm*)this->topLevelWidget() )->removeTab(objectName());
}




void dEditJournal::typeChange()
{
    if ( cbType->currentIndex() != 1 )
	tabWidget18->removeTab(tabWidget18->indexOf(tabWidget18->widget(2)));
    else
	tabWidget18->addTab(docPage,QObject::tr("Documents"));
}


void dEditJournal::addDoc()
{
    ananasListViewItem *cur = (ananasListViewItem *)allDocs->currentItem();
    if (!cur)
	return;
    if ( md->objClass( cur->obj ) != md_document )
	return;
    new ananasListViewItem(journalDocs, journalDocs->topLevelItem( journalDocs->topLevelItemCount() - 1 ), md, cur->obj );
}


void dEditJournal::removeDoc()
{
    delete journalDocs->currentItem();
}


void dEditJournal::moveUp()
{
    ananasListViewItem *aitem, *after;

    aitem = (ananasListViewItem *) journalDocs->currentItem();
    if ( aitem )
    {
	after = aitem->previousSibling();
	if ( after )
	{
	    int idx = journalDocs->indexOfTopLevelItem( after );
	    QTreeWidgetItem *moved = journalDocs->takeTopLevelItem( idx );
	    journalDocs->insertTopLevelItem( journalDocs->indexOfTopLevelItem( aitem ) + 1, moved );
	}
    }
}


void dEditJournal::moveDown()
{
    ananasListViewItem *aitem, *after;
    aitem = (ananasListViewItem *)journalDocs->currentItem();
    if ( aitem )
    {
	after = aitem->nextSibling();
	if ( after )
	{
	    int idx = journalDocs->indexOfTopLevelItem( aitem );
	    QTreeWidgetItem *moved = journalDocs->takeTopLevelItem( idx );
	    journalDocs->insertTopLevelItem( journalDocs->indexOfTopLevelItem( after ) + 1, moved );
	}
    }
}
