#include "deditcommand.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include <qstatusbar.h>
//Added by qt3to4:
#include <QPixmap>
#include <QIcon>
#include "acfg.h"

//extern aCfg cfg;

/*
 *  Constructs a dEditCommand as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditCommand::dEditCommand(QWidget* parent, const char* name, Qt::WindowFlags fl)
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
dEditCommand::~dEditCommand()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditCommand::languageChange()
{
    retranslateUi(this);
}

void dEditCommand::updateMD()
{
    aCfgItem com_action, apix;
    al->updateMD( );
    QPixmap pix;

    item->setText( 0, eName->text().trimmed() );
    md->setAttr( obj, mda_name, eName->text().trimmed() );
    md->setSText( obj, md_description, eDescription->toPlainText() );
    md->setSText( obj, md_menutext, eMenuText->text() );

    QString sKey = "";
    if ( cbKey-> currentText () != "" )
    {
	if ( cbCTRL->isChecked() )
	    sKey += md_km_ctrl;
	if ( cbALT->isChecked() )
	    sKey += md_km_alt;
	if ( cbShift->isChecked() )
	    sKey += md_km_shift;
	sKey += cbKey->currentText ();
    }
    md->setSText( obj, md_key, sKey );

    do
	{
		com_action = md->findChild( obj, md_comaction, 0 );
		if ( !com_action.isNull() )
			 md->remove( com_action );
    } while ( !com_action.isNull() );
    ananasListViewItem *aitem = (ananasListViewItem *)vComActions->topLevelItem(0);
	if (!aitem)
		return;
    while ( aitem )
	{
		com_action = md->insert( obj, md_comaction, QString(), -1 );
		md->setText( com_action, QString( "%1" ).arg( aitem->id ) );
		aitem = (ananasListViewItem *)aitem->nextSibling();
    }
    aitem = (ananasListViewItem *)vComActions->topLevelItem(0);
    apix = md->findChild( md->find( aitem->id ), md_active_picture, 0 );
	if ( apix.isNull() )
		return;
    pix.loadFromData( md->binary( apix ) );
    item->setIcon( 0, QIcon(pix) );
}

void dEditCommand::destroy()
{
    updateMD();
    ( (MainForm*)this->window() )->wl->remove( this );
    ( (MainForm*)this->window() )->removeTab(objectName());
}

void dEditCommand::init()
{
	statusBar()->hide();
}

void dEditCommand::setData( InterfaceListViewItem * o )
{
    int i, j, n, k, id;
    item = o;
    md = o->md;
    obj = o->obj;
    aCfgItem  com_action, apix;
    aAliasEditor *a = new aAliasEditor( md, obj, tAliases );
    QPixmap pix;

	vComActions = new ananasTreeView( tabWidget2->widget(1),  md );
    vComActions->setSortingEnabled( false );
    // TODO Fixme!!!
    //--layout28->addWidget( vComActions, 0, 0 );
    actiontree = new aActionTreeView ( tabWidget2->widget(1),  md );
    disconnect( actiontree, SIGNAL( customContextMenuRequested( const QPoint& ) ), actiontree, SLOT(ContextMenu() ) );
    disconnect( actiontree, SIGNAL( itemActivated( QTreeWidgetItem*, int ) ), actiontree, SLOT( itemEdit() ) );
    disconnect( actiontree, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), actiontree, SLOT( itemEdit() ) );
    // TODO Fixme!!!
    //--layout29->addWidget( actiontree, 0, 0 );

    al = a;
    al->setData();

    setWindowTitle( tr("Command:") + md->attr( obj, mda_name ) );
    eName->setText( md->attr( obj, mda_name ) );
    eMenuText->setText( md->sText( obj, md_menutext ) );
    eDescription->setPlainText( md->sText( obj, md_description ) );

    QString sKey = md->sText( obj, md_key );
	if ( ( sKey.indexOf (md_km_ctrl) ) >= 0 )
	{
		cbCTRL->setChecked ( true );
		sKey.remove(md_km_ctrl);
	}
	if ( ( sKey.indexOf (md_km_alt) ) >= 0 )
	{
		cbALT->setChecked ( true );
		sKey.remove(md_km_alt);
	}
	if ( ( sKey.indexOf (md_km_shift) ) >= 0 )
	{
		cbShift->setChecked ( true );
		sKey.remove(md_km_shift);
	}


	n = cbKey->count();
	for ( i = 0; i < n; i++ )
		if ( sKey == cbKey->itemText( i ) )
		{
			cbKey->setCurrentIndex( i );
			break;
		}
    n = md->countChild( obj, md_comaction );
	k = md->count( md->find ( mdc_actions ), md_action );
	ananasListViewItem *aitem;
	aCfgItem actionObj;
	for ( i = 0; i < n; i++ )
	{
		actionObj = md->find (obj, md_comaction, i);
		id = md->text(actionObj).toLong();
		if ( actionObj.isNull() )
			md->remove(actionObj);
		com_action = md->find(id);
		aitem = new ananasListViewItem (vComActions, vComActions->topLevelItem(vComActions->topLevelItemCount() - 1), md, com_action);
		aitem->setFlags( aitem->flags() & ~Qt::ItemIsEditable );
		apix = md->findChild( com_action, md_active_picture, 0 );
		if ( apix.isNull() )
			break;
		pix.loadFromData( md->binary( apix ) );
		aitem->setIcon( 0, QIcon(pix) );
/*		for ( j = 0; j < k; j++)
		{
			com_action = md->find ( md->find( mdc_actions ), md_action, j  );
			if ( md->id(com_action) == id )
			{
				aitem = new ananasListViewItem (vComActions, vComActions->lastItem(), md, com_action);
				aitem->setRenameEnabled( 0, false );
				apix = md->findChild( com_action, md_active_picture, 0 );
				if ( apix.isNull() )
					break;
				pix.loadFromData( md->binary( apix ) );
				aitem->setPixmap( 0, pix );
			}
		}*/
    }
}


void
dEditCommand::bAddAction_clicked()
{
    aCfgItem apix;
    QPixmap pix;

    ananasListViewItem *cur = (ananasListViewItem *)actiontree->currentItem();
	if (!cur)
		return;
	if ( md->objClass( cur->obj ) != md_action )
		return;
	ananasListViewItem *aitem = new ananasListViewItem(vComActions, vComActions->topLevelItem(vComActions->topLevelItemCount() - 1), md, cur->obj );
	apix = md->findChild( md->find( aitem->id ), md_active_picture, 0 );
	pix.loadFromData( md->binary( apix ) );
	aitem->setIcon( 0, QIcon(pix) );
}


void dEditCommand::bRemoveAction_clicked()
{
	delete vComActions->currentItem();
}


void dEditCommand::bMoveUp_clicked()
{
    ananasListViewItem *aitem;

    aitem = (ananasListViewItem *) vComActions->currentItem();
    if ( aitem )
	{
		int idx = vComActions->indexOfTopLevelItem( aitem );
		if ( idx > 0 )
		{
			vComActions->takeTopLevelItem( idx );
			vComActions->insertTopLevelItem( idx - 1, aitem );
			vComActions->setCurrentItem( aitem );
		}
	}
}


void dEditCommand::bMoveDown_clicked()
{
    ananasListViewItem *aitem;
    aitem = (ananasListViewItem *)vComActions->currentItem();
    if ( aitem )
	{
		int idx = vComActions->indexOfTopLevelItem( aitem );
		if ( idx >= 0 && idx + 1 < vComActions->topLevelItemCount() )
		{
			vComActions->takeTopLevelItem( idx );
			vComActions->insertTopLevelItem( idx + 1, aitem );
			vComActions->setCurrentItem( aitem );
		}
	}
}
