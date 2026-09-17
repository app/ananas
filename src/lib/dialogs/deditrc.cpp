#include "deditrc.h"

#include <QFileDialog>
//#include <qstring.h>
#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include "atests.h"
/*
 *  Constructs a dEditRC as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
dEditRC::dEditRC(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    Q_UNUSED(name);
    setModal(modal);
    setupUi(this);

}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditRC::~dEditRC()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditRC::languageChange()
{
    retranslateUi(this);
}



void dEditRC::setdata(QString nameRC, rcListViewItem *item)
{
//	QFile f;
//	QString s, sv, sn;
//	char buf[1025];
	it = item;
	QMap<QString,QString> cfg;

	cfg = aTests::readConfig(QDir::convertSeparators(nameRC));
	eRCFile->setText(QDir::convertSeparators(nameRC));
	eDBType->setCurrentIndex(0);

	eDBTitle->setText(cfg["dbtitle"]);
	eDBName->setText(cfg["dbname"]);
	eDBUserName->setText(cfg["dbuser"]);
	ePass->setText(cfg["dbpass"]);
	eDBHost->setText(cfg["dbhost"]);
	eDBPort->setText(cfg["dbport"]);
	eCfgName->setText(QDir::convertSeparators(cfg["configfile"]));
	if(cfg["dbtype"]=="postgres") eDBType->setCurrentIndex(3);
	if(cfg["dbtype"]=="mysql") eDBType->setCurrentIndex(2);
	if(cfg["dbtype"]=="internal") eDBType->setCurrentIndex(1);
	eWorkDir->setText(QDir::convertSeparators(cfg["workdir"]));
}


void dEditRC::updatecfg()
{
	QMap<QString,QString> cfg;

	cfg["dbtitle"]	= eDBTitle->text();
	cfg["dbname"]	= eDBName->text();
	if(eDBType->currentIndex()==1) cfg["dbtype"] ="internal";
	if(eDBType->currentIndex()==2) cfg["dbtype"] ="mysql";
	if(eDBType->currentIndex()==3) cfg["dbtype"]= "postgres";
	cfg["dbuser"]	= eDBUserName->text();
	cfg["dbpass"]	= ePass->text();
	cfg["dbhost"]	= eDBHost->text();
	cfg["dbport"]	= eDBPort->text();
	cfg["workdir"]	= QDir::convertSeparators(eWorkDir->text());
	cfg["configfile"]= QDir::convertSeparators(eCfgName->text());

	aTests::writeConfig(QDir::convertSeparators(eRCFile->text()),cfg);
}



void dEditRC::onOK()
{
	updatecfg();
	it->rcfile = QDir::convertSeparators(eRCFile->text());
	it->setText(0,eDBTitle->text());
	accept();
}


void dEditRC::onRCFile()
{
		QFileDialog fd( 0, tr("Select ananas config resource"),
  			QString(), tr("ananas config resource (*.rc)") );
		fd.setFileMode ( QFileDialog::AnyFile );
		fd.selectFile( QDir::convertSeparators(eRCFile->text()));
		if ( fd.exec() == QDialog::Accepted ) {
			eRCFile->setText(QDir::convertSeparators(fd.selectedFiles().value(0)));
			setdata(eRCFile->text(),it);
		} else {
			return;
		}
}


void dEditRC::onCFGFile()
{
		QFileDialog fd( 0, tr("Select ananas config file"),
  			QString(), tr("ananas config file (*.cfg)") );
		fd.setFileMode ( QFileDialog::AnyFile );
		fd.selectFile( QDir::convertSeparators(eCfgName->text()));
		if ( fd.exec() == QDialog::Accepted ) {
			eCfgName->setText(QDir::convertSeparators(fd.selectedFiles().value(0)));
		} else {
			return;
		}
}
