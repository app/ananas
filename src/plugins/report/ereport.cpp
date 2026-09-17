#include "ereport.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include "acfg.h"

/*
 *  Constructs a eReport as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
eReport::eReport(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    Q_UNUSED(name);
    setModal(modal);
    setupUi(this);

    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
eReport::~eReport()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void eReport::languageChange()
{
    retranslateUi(this);
}

void eReport::init()
{
/*
	int oc, i, f, idx=0;
	char *id, *name, *ot;
	cfg_objptr o;
	char otype[100];
	QString named;

	otypes.clear();
	eType->clear();
	otypes.append("D");
	eType->insertItem(idx++, tr("Дата"));
	otypes.append("T");
	eType->insertItem(idx++, tr("Время"));
	otypes.append("N %d %d");
	eType->insertItem(idx++, tr("Число"));
	otypes.append("C %d");
	eType->insertItem(idx++, tr("Строка"));

	oc=cfgobj_count(NULL, NULL);
	for (i=1;i<=oc;i++) {
		id=cfgobj_idn(NULL, NULL, i, &o);
		ot=(char *)o->name;
		name=cfgobj_attr(o, "name");
		named="";
		f=0;
		if (strcmp((char *)ot, aot_doc)==0) {
			named=tr("Документ.");
			f=1;
		}
		if (strcmp((char *)ot, aot_cat)==0) {
			named=tr("Справочник.");
			f=1;
		}
		if (f) {
			named=named+tr(name);
			sprintf(otype, "O %s", id);
			otypes.append(otype);
			eType->insertItem(idx++, named);
		}
	}
*/
}



/*void eReport::setData( aCatalogue *f, QWidget *mw )
{
	aCfg *md = 0;
	if ( !f ) return;
//	printf("name = '%s'\n", (const char *) mw->objectName() );
	if (mw->objectName() == QString("ananas-designer_mainwindow") ) {
//		printf("running under ananas designer\n");
		CfgForm *cf = ( ( MainForm *) mw )->cfgForm();
		if ( cf ) {
			md = &cf->cfg;
//			QString ts = f->getFieldType();
//			char t=' ';
			int w=0, d=0, idx=0;
			unsigned int i;
			long oid , id;

			id = f->getId();

			otypes.clear();
			eType->clear();

			QStringList tlist = md->types( md_catalogue );
			otypes.clear();
			eType->clear();
			for ( QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it ) {
				otypes.append( (*it).section( "\t", 0, 0 ) );
				eType->insertItem(idx++,  (*it).section("\t", 1, 1 ));
			}
			for ( i = 0 ; i < otypes.count(); i++ ) {
				oid = 0;
				if( otypes[i][0] == 'O' ) {
					sscanf( otypes[i].toLatin1().constData(), "O %d", &oid );
					if ( oid == id ) {
						eType->setCurrentIndex( i );
						break;
					}
				}
			}
		} else reject();
	}

}
*/

void
eReport::setData( QWidget *o, aCfg *md )
{
//    const QObject *o = sender();
    if ( o ) {
	if ( o->metaObject()->className() != QString("wReport") || !md ) {
	    reject();
	    return;
	}
    }
    else {
	reject();
	return;
    }
    wReport *f = ( wReport*) o;
    int w=0, d=0, idx=0;
    unsigned int i;
    long oid , id;

    id = f->getId();

    QStringList tlist = md->types( md_journal );
    otypes.clear();
    eType->clear();
    for ( QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it ) {
	otypes.append( (*it).section( "\t", 0, 0 ) );
	eType->insertItem(idx++,  (*it).section("\t", 1, 1 ));
    }
    for ( i = 0 ; i < otypes.count(); i++ ) {
	oid = 0;
	if( otypes[i][0] == 'O' ) {
	    sscanf( otypes[i].toLatin1().constData(), "O %d", &oid );
	    if ( oid == id ) {
		eType->setCurrentIndex( i );
		break;
	    }
	}
    }
}


void eReport::getData( QWidget * o )
{

/*	int idx=eType->currentIndex();
	long oid = 0;

	if (f) {
		if( otypes[idx][0] == 'O' ) {
			sscanf( otypes[idx].toLatin1().constData(), "O %d", &oid );
			f->setId( oid );
		}
	}
*/
//    const QObject *o = sender();
    if ( !o ) return;
    if ( o->metaObject()->className() != QString("wReport") ) return;
    wReport *f = ( wReport*) o;

    int idx=eType->currentIndex();
    long oid = 0;

    if (f) {
	if( otypes[idx][0] == 'O' ) {
	    sscanf( otypes[idx].toLatin1().constData(), "O %d", &oid );
	    f->setId( oid );
	}
    }

}
