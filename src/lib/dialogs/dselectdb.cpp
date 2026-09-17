#include "dselectdb.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include "atests.h"
#include "dhelpcfgform.h"
/*
 *  Constructs a dSelectDB as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
dSelectDB::dSelectDB(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, name, modal, fl)
    ,settings( QSettings::IniFormat, QSettings::UserScope, "Ananas company Ltd", "Ananas4")
{
    setupUi(this);

    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dSelectDB::~dSelectDB()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dSelectDB::languageChange()
{
    retranslateUi(this);
}


//#include "acfgobj.h"
#include <qdir.h>
#include <qstring.h>
#include <QFileDialog>
#include <qlineedit.h>
#include <qstring.h>
#include <qdom.h>
//Added by qt3to4:
#include <QMenu>

#include "deditrc.h"
#include "dimportdb.h"
#include "acfgrc.h"
#include "atests.h"
#include "adataexchange.h"
#include "alog.h"
#include <QTreeWidgetItem>

static QTreeWidgetItem *nextSiblingItem( QTreeWidgetItem *item )
{
	if ( !item ) return 0;
	QTreeWidgetItem *p = item->parent();
	if ( p ) return p->child( p->indexOfChild( item ) + 1 );
	QTreeWidget *t = item->treeWidget();
	return t ? t->topLevelItem( t->indexOfTopLevelItem( item ) + 1 ) : 0;
}

static int itemDepth( QTreeWidgetItem *item )
{
	int d = 0;
	while ( item->parent() ) { item = item->parent(); ++d; }
	return d;
}

void dSelectDB::init()
{
	createMenu();
	progressBar1->hide();
	QString fname;
	withgroups = 1;
	//--settings.insertSearchPath( QSettings::Unix, QString(QDir::homeDirPath())+QString("/.ananas"));
	//--settings.insertSearchPath( QSettings::Windows, "/ananasgroup/ananas" );

	local = settings.entryList("/groups").count();
	changes = false;
	/*--if(!local)
	{

		aLog::print(aLog::Debug, tr("dSelectDB local settings not found"));
		settings.insertSearchPath( QSettings::Windows, "/ananasgroup/ananas/globalsettings");
#ifdef Q_OS_WIN32
		QStringList subkeys = settings.subkeyList("/ananasgroup/ananas/globalsettings");
		settings.insertSearchPath(QSettings::Windows,"/ananasgroup/ananas/globalsettings");
		QStringList entryGroup;
		for(uint i=0; i<subkeys.count();i++)
		{
			QString str =subkeys[i];
			//str = str.left(str.length()-2); // length("rc")=2
			entryGroup =settings.entryList("/"+str);
			settings.beginGroup("/"+str);
			readSettings(entryGroup);
			settings.endGroup();
		}

#else
		QString suff = "*_grouprc";
		QDir dir("/etc/ananas",suff,QDir::Name | QDir::IgnoreCase, QDir::Files | QDir::Readable);
		QStringList subkeys = dir.entryList(suff);
		settings.insertSearchPath(QSettings::Unix,"/etc/ananas/");
		QStringList entryGroup;
		for(uint i=0; i<subkeys.count();i++)
		{
			QString str =subkeys[i];
			str = str.left(str.length()-2); // length("rc")=2
			entryGroup =settings.entryList("/"+str);
			settings.beginGroup("/"+str);
			readSettings(entryGroup);
			settings.endGroup();
		}
#endif
	}
	else*/
	{
		QStringList lst = settings.entryList("/groups");
		settings.beginGroup("/groups");
		readSettings(lst);
		settings.endGroup();
	}
	setWindowIcon( rcIcon("a-system.png"));
	listDBRC->hideColumn( 1 );
	listDBRC->setSortingEnabled( false );
	listDBRC->header()->hide();
	listDBRC->setRootIsDecorated(1);
	buttonOk->setEnabled( FALSE );
	return;
}

void dSelectDB::readSettings(QStringList entryGroup)
{
	rcfile="";
	ro_mode=0;
	QFile f, fc;
	QString s, sp, sn, rc;
	rcListViewItem *item, *gitem=NULL; //, *subitem;
	for(uint j=0; j<entryGroup.count();j++)
	{
        	QStringList eitems;
		eitems = settings.entryList(entryGroup[j]);
		QString groupName = "unknown group";
		groupName = settings.readEntry(entryGroup[j]);
		aLog::print(aLog::Debug, tr("dSelectDB read settings for group with name %1").arg(groupName));
		rcListViewItem * lastIt = (rcListViewItem *) listDBRC->topLevelItem( listDBRC->topLevelItemCount()-1 );
		if(lastIt!=NULL)
		{
			while(itemDepth(lastIt)>0)
			{
				lastIt =(rcListViewItem *) lastIt->parent();
			}
			gitem= new rcListViewItem(listDBRC, lastIt, groupName, "", true );
			gitem->setExpanded(true);
		}
		else
		{
			gitem= new rcListViewItem(listDBRC, groupName, "", true );
			gitem->setExpanded(true);
		}
//		withgroups=1;
		QMap<QString,QString> cfg;
		for(int k = eitems.count()-1; k>=0; k--)
		{
			if(k<0) break;
			rc = settings.readEntry(entryGroup[j]+"/"+eitems[k]);
			cfg = aTests::readConfig(QDir::convertSeparators(rc));
			sn=cfg["dbtitle"];
			if (gitem) item= new rcListViewItem( gitem, sn, rc );
			else item= new rcListViewItem( listDBRC, sn, rc );
		}
	}
}


void dSelectDB::itemSelect()
{
	rcListViewItem *i = ( rcListViewItem *) listDBRC->currentItem();
	if (! i ) return;
	buttonOk->setEnabled( !i->group );
}


void dSelectDB::newGroup()
{
	QTreeWidgetItem *gitem;
	gitem = new rcListViewItem(listDBRC, tr("New group"), "", true );
	listDBRC->setRootIsDecorated(1);
	gitem->setExpanded(true);
	listDBRC->setCurrentItem(gitem);
	gitem->setSelected(true);
	editItem();
	withgroups=1;
	changes = true;
	//saveRC();
}


void dSelectDB::newItem()
{
	rcListViewItem *item, *gitem;
	QString rc;
	item= ( rcListViewItem *) listDBRC->currentItem();
	if (!item) return;
	if (withgroups) {
		if (item->parent()) gitem = ( rcListViewItem *) item->parent();
		else gitem=item;
		item = new rcListViewItem(gitem, tr("New shema"), "myrc");
		gitem->setExpanded(true);

	} else {
		item = new rcListViewItem(listDBRC, tr("New shema"), "myrc");
	}
	listDBRC->setCurrentItem(item);
	item->setSelected(true);
	editItem();
	changes = true;
}


void dSelectDB::editItem()
{
	rcListViewItem *item;
	dEditRC *d = new dEditRC(this);

	item = ( rcListViewItem *) listDBRC->currentItem();
	if (!item) return;
	if ( item->group )
	{
	// Group
		item->setFlags( item->flags() | Qt::ItemIsEditable );
		listDBRC->editItem( item, 0 );
		changes = true;
	}
	else
	{
	// Resource
		d->setdata( QDir::convertSeparators(item->rcfile), item );
		if ( d->exec() == QDialog::Accepted )
		{
			changes = true;
		}
	}
}



void dSelectDB::deleteItem()
{
	QTreeWidgetItem *item;
	item=listDBRC->currentItem();
	if (item)
	{
		QString msg = tr("Delete item?");
		if(QMessageBox::question(this,tr("confirm delete"),msg,QMessageBox::Ok,QMessageBox::Cancel)!=QMessageBox::Ok)
		{
			return;
		}

		delete item;
		if (listDBRC->topLevelItemCount()==0) withgroups=0;
		changes = true;
	}
}


void dSelectDB::saveRC()
{
	rcListViewItem *item, *gitem;
	QFile f( rcfile );
	aLog::print(aLog::Info, tr("dSelectDB save settings"));
	if (!local)
	{
		if(!changes) return;
		QString home = QDir::homeDirPath();
#ifndef Q_OS_WIN32
		QString msg = QString("Все внесенные изменения будут сохранены локально в вашем домашнем каталоге.\nПри очередном запуске программы будут использованы локальные настройки из каталога\n%1/.ananas.\nЕсли Вы хотите использовать глобальные настройки, вам следует удалить каталог\n%2/.ananas").arg(home).arg(home);
#else
		QString msg = tr("save settings local?");
#endif
		if(QMessageBox::question(this,tr("save"),QString::fromUtf8(msg),QMessageBox::Ok,QMessageBox::Cancel)!=QMessageBox::Ok)
		{
			return;
		}
	}
		//--settings.removeSearchPath( QSettings::Unix, "/etc/ananas" );
		//--settings.removeSearchPath( QSettings::Windows, "/ananasgroup/ananas/globalsettings");


		gitem= ( rcListViewItem *) listDBRC->topLevelItem(0);
		uint gcount=0;
		uint ecount=0;
		clearSettings();

		settings.beginGroup("/groups");

		while (gitem)
		{
			if (withgroups)
			{
                                settings.writeEntry(QString::number(gcount),gitem->text(0));
				if(gitem->childCount())
				{
					item = ( rcListViewItem *) gitem->child(0);
					while (item)
					{
						settings.writeEntry(QString::number(gcount)+"/"+QString::number(ecount),item->rcfile);
						item =  ( rcListViewItem *) nextSiblingItem(item);
						++ecount;
					}
				}
			}
			++gcount;
			gitem =  ( rcListViewItem *) nextSiblingItem(gitem);
		}
		settings.endGroup();
}


void dSelectDB::onOK()
{
	rcListViewItem *item;

	saveRC();
	item =  ( rcListViewItem *) listDBRC->currentItem();
	if (item) {
	             if ( !item->rcfile.isEmpty() ) {
			 rcfile = item->rcfile;
			 accept();
		}
	}
}


void dSelectDB::onCancel()
{
	//saveRC();
	reject();
}


void dSelectDB::ItemRenamed(QTreeWidgetItem *item, int col)
{
	if (!item) return;
	if (col>1) return;
}


void dSelectDB::clearSettings()
{
        QStringList eitems;
	QStringList entryGroup =settings.entryList("/groups");
	settings.beginGroup("/groups");
	for(uint j=0; j<entryGroup.count();j++)
	{
		eitems = settings.entryList(entryGroup[j]);
		settings.removeEntry(entryGroup[j]);
		for(int k = eitems.count()-1; k>=0; k--)
		{
			if(k<0) break;
			settings.removeEntry(entryGroup[j]+"/"+eitems[k]);
		}
	}
	settings.endGroup();
}


void dSelectDB::onHelp()
{
	HelpCfgForm f;
	f.exec();
}

void dSelectDB::createMenu()
{

	menuBar = new QMenuBar(this);
	QMenu *menuFile = new QMenu(tr("Program"), this);
	menuFile->addAction(tr("E&xit"), this, SLOT(onCancel()));

	QMenu *menuEdit = new QMenu(tr("Actions"), this);
	menuEdit->addAction(tr("New &group"), this, SLOT(newGroup()));
	menuEdit->addAction(tr("&New shema"), this, SLOT(newItem()));
	menuEdit->addSeparator();
	menuEdit->addAction(tr("Edi&t"),  this, SLOT(editItem()));
	menuEdit->addSeparator();
	menuEdit->addAction(tr("&Delete"),  this, SLOT(deleteItem()));

	QMenu *menuAction = new QMenu(tr("Service"), this);
	menuAction->addAction(tr("&Import"),  this, SLOT(importItem()));
	menuAction->addAction(tr("&Export"),  this, SLOT(exportItem()));

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuEdit);
	menuBar->addMenu(menuAction);
	layout()->setMenuBar(menuBar);
	menuBar->show();
}


void dSelectDB::importItem()
{
	rcListViewItem *item,*gitem;
	dImportDB *d = new dImportDB(this);
	item = (rcListViewItem *)listDBRC->currentItem();
	if (!item) return;
	if (withgroups)
	{
		if (item->parent()) gitem = ( rcListViewItem *) item->parent();
		else gitem=item;
		item = new rcListViewItem(gitem, tr("New shema"), "myrc");
		gitem->setExpanded(true);

	}
	else
	{
		item = new rcListViewItem(listDBRC, tr("New shema"), "myrc");
	}
	listDBRC->setCurrentItem(item); item->setSelected(true);
	d->setdata("", item );
	if ( d->exec() == QDialog::Accepted )
	{
		bool res;
		QString rcfile = item->rcfile;
		QString filename = d->eCfgName->text();
		progressBar1->show();
		progressBar1->reset();
		aBackup newBase;
		connect (&newBase, SIGNAL(progress(int,int)), progressBar1, SLOT( setProgress(int,int)));
		if(newBase.importData(rcfile, filename, true)==false)
		{
			QMessageBox::information(this,tr("Backup"),newBase.lastError(),Qt::NoButton);
			progressBar1->hide();
			aLog::print(aLog::Info, tr("dSelectDB business schema import"));
		}
		else
		{
			aLog::print(aLog::Error, tr("dSelectDB restore: %1").arg(newBase.lastError()));
			QMessageBox::critical(this,tr("Backup"),newBase.lastError(),0, Qt::NoButton);
			progressBar1->hide();
			delete item;
			item =0;
			return;
		}



		changes = true;
	}
	else
	{
		delete item;
		item = 0;
		return;
	}
}


void dSelectDB::exportItem()
{
//	progressBar1->setTotalSteps(10);
	QString dir,rcfile,filename;
#ifndef Q_OS_WIN32
	dir = "/home";
#endif

	rcListViewItem *item =  ( rcListViewItem *) listDBRC->currentItem();
	if (item)
	{
		if ( !item->rcfile.isEmpty() && !item->group )
		{
			rcfile = item->rcfile;
		}
		else
		{
			QMessageBox::information(this,tr("Select item"),tr("Please, select item for export"),Qt::NoButton);
			return;
		}

		QFileDialog *fdlg = new QFileDialog(this); // create modal dialog
		fdlg->setObjectName( "fileDialog" );
		fdlg->setFileMode ( QFileDialog::AnyFile );
		if(fdlg->exec()==QDialog::Accepted)
		{
			filename = fdlg->selectedFiles().value(0);
		}
		else
		{
			return;
		}
		delete fdlg;
		fdlg = 0;

		aBackup backupBase;
		progressBar1->show();
		progressBar1->reset();
		connect (&backupBase, SIGNAL(progress(int,int)), progressBar1, SLOT( setProgress(int,int)));
		if(backupBase.exportData(rcfile, filename, true )==0)
		{
			aLog::print(aLog::Info, tr("dSelectDB backup ok"));
			QMessageBox::information(this,tr("Backup"),backupBase.lastError(),Qt::NoButton);
			progressBar1->hide();

		}
		else
		{
			aLog::print(aLog::Debug, tr("dSelectDB backup: %1").arg(backupBase.lastError()));
			QMessageBox::critical(this,tr("Backup"),backupBase.lastError(),0, Qt::NoButton);
			progressBar1->hide();
			return;
		}
	}
	else
	{
		QMessageBox::information(this,tr("Select item"),tr("Please, select item for export"),Qt::NoButton);
		return;
	}

}


void dSelectDB::onDblClick( QTreeWidgetItem *item, int )
{

	if (! (( rcListViewItem *)item)->group )
	{
		onOK();

	}
}

