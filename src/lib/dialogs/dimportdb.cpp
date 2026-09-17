/****************************************************************************
** Form implementation generated from reading ui file 'deditrc.ui'
**
** Created: Пнд Фев 13 14:29:13 2006
**      by: The User Interface Compiler ($Id: dimportdb.cpp,v 1.1 2008/11/05 21:16:28 leader Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "dimportdb.h"
#include "atests.h"
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <QFileDialog>

dImportDB::dImportDB( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : dEditRC( parent, name, modal, fl )
{
    if ( !name )
	setName( "dImportDB" );
    languageChange();

}

/*
 *  Destroys the object and frees any allocated resources
 */
dImportDB::~dImportDB()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void
dImportDB::languageChange()
{
	dEditRC::languageChange();
	setCaption( tr( "Resource file" ) );
	textLabel8_2->setText( tr( "archive file" ) );
	bOK->setText( tr( "Import" ) );
}

void
dImportDB::onCFGFile()
{
		QFileDialog fd( 0, tr("Select business schema archive"),
  			QString(),
			tr("ananas business schema archiff file (*.bsa);;any files (*)") );
		fd.setFileMode(QFileDialog::AnyFile);
		fd.selectFile( QDir::convertSeparators(eCfgName->text()));
		if ( fd.exec() == QDialog::Accepted )
		{
			eCfgName->setText(QDir::convertSeparators(fd.selectedFiles().value(0)));
		}
		else
		{
			return;
		}
}
