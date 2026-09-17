#include "edocument.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

#include "acfg.h"

/*
 *  Constructs a eDocument as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
eDocument::eDocument(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
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
eDocument::~eDocument()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void eDocument::languageChange()
{
    retranslateUi(this);
}

void eDocument::init()
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
	eType->insertItem(idx++, trUtf8("Дата"));
	otypes.append("T");
	eType->insertItem(idx++, trUtf8("Время"));
	otypes.append("N %d %d");
	eType->insertItem(idx++, trUtf8("Число"));
	otypes.append("C %d");
	eType->insertItem(idx++, trUtf8("Строка"));

	oc=cfgobj_count(NULL, NULL);
	for (i=1;i<=oc;i++) {
		id=cfgobj_idn(NULL, NULL, i, &o);
		ot=(char *)o->name;
		name=cfgobj_attr(o, "name");
		named="";
		f=0;
		if (strcmp((char *)ot, aot_doc)==0) {
			named=trUtf8("Документ.");
			f=1;
		}
		if (strcmp((char *)ot, aot_cat)==0) {
			named=trUtf8("Справочник.");
			f=1;
		}
		if (f) {
			named=named+trUtf8(name);
			sprintf(otype, "O %s", id);
			otypes.append(otype);
			eType->insertItem(idx++, named);
		}
	}
*/
}



void eDocument::setData( QWidget *o, aCfg *md )
{
//	const QObject *o = sender();
	if ( o ) {
	    if ( o->metaObject()->className() != QString("wDocument") || !md ) {
		reject();
		return;
	    }
	}
	else {
		reject();
		return;
	}
	wDocument *f = ( wDocument*) o;
			int w=0, d=0, idx=0;
			unsigned int i;
			long oid , id;

			id = f->getId();

			otypes.clear();
			eType->clear();

			QStringList tlist = md->types( md_document );
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


void eDocument::getData( QWidget *o )
//aDocument *f )
{
	QVariant v;
//	const QObject *o = sender();
            if ( !o ) return;
	if ( o->metaObject()->className() != QString("wDocument") ) return;
	wDocument *f = ( wDocument*) o;

	int idx=eType->currentIndex();
	int oid = 0;

	if (f) {
		if( otypes[idx][0] == 'O' ) {
			sscanf( otypes[idx].toLatin1().constData(), "O %d", &oid );
//			v = oid;
			f->setProperty("Id", QVariant( oid ) );
//			f->setId( oid );
		}
	}

}

