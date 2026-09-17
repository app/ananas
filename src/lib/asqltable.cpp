/****************************************************************************
** $Id: asqltable.cpp,v 1.2 2009/05/25 16:35:10 app Exp $
**
** Code file of the Ananas database table of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
**
** This file is part of the Library of the Ananas
** automation accounting system.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.leaderit.ru/page=ananas or email sales@leaderit.ru
** See http://www.leaderit.ru/gpl/ for GPL licensing information.
**
** Contact org@leaderit.ru if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "asqltable.h"
#include <qdatetime.h>
#include <QSqlError>
#include <QSqlDriver>
#include <QSqlDatabase>
#include <QVariant>
#include <QStringList>
#include "adatabase.h"

#include "acatalogue.h"
#include "adocument.h"
#include "aaregister.h"
#include "alog.h"

/*!
 *	\~english
 *	Constructor
 *	\~russian
 *	Создает таблицу доступа к базе данных, позволяющую работать с одной
 *	sql таблицей или одним sql представлением (view).
 *	\param context - объект метаданных, который описывает таблицу.
 *	\param adb - ссылка на объект базы данных, которой
 *	принадлежит sql таблица.
 *	\~
 */
aDataTable::aDataTable( aCfgItem context, aDatabase *adb )
{
	db = adb;
	md = &db->cfg;
	mdobjId = 0;
	selected = false;
	m_index = -1;
	m_readOnly = false;
	tableName = db->tableDbName( db->cfg, context, &mdobjId );
	if ( !tableName.isEmpty() ) {
		QSqlDatabase *d = db->db();
		if ( d && d->driver() )
			m_dbRecord = d->driver()->record( tableName );
		m_schema = m_dbRecord;
		init( context, adb );
	}
}



/*!
 *	\~english
 *	Constructor
 *	\~russian
 *	Создает таблицу доступа к базе данных, позволяющую работать с одной
 *	sql таблицей или одним sql представлением (view).
 *	\param tname - имя sql таблицы в базе данных
 *	\param adb - ссылка на объект базы данных, которой
 *	принадлежит sql таблица.
 *	\~
 *
 */
aDataTable::aDataTable( const QString &tname, aDatabase *adb )
{
	db = adb;
	md = &db->cfg;
	tableName = tname;
	mdobjId = 0;
	selected = false;
	m_index = -1;
	m_readOnly = false;
	QSqlDatabase *d = db->db();
	if ( d && d->driver() )
		m_dbRecord = d->driver()->record( tname );
	m_schema = m_dbRecord;
}

/*!
 *	\~english
 *	Destructor
 *	\~russian
 *	Деструктор
 *	\~
 *
 */
static void clearObjectHash( QHash<QString, QObject*> &h )
{
	for ( QHash<QString, QObject*>::iterator it = h.begin(); it != h.end(); ++it )
		delete it.value();
	h.clear();
}

aSQLTable::~aSQLTable()
{
	clearObjectHash( p_reg );
	clearObjectHash( p_cat );
	clearObjectHash( p_doc );
}

/*!
 *	\~english
 *	Inits table object.
 *	\~russian
 *	Инициализирует объект.
 *	\~
 */
void
aDataTable::init( aCfgItem context, aDatabase *adb )
{
	db = adb;
	fnames.clear();
	userFilter.clear();
	setObject( context );
}



/*!
 *	\~english
 *	Sets md object to table.
 *	\~russian
 *	Задает объект метаданных для таблицы.
 *	\~
 */
void
aDataTable::setObject( aCfgItem context )
{
	aCfgItem cobj, parent;
	parent = obj = context;


	mdobjId = md->id(obj);
	while ( !mdobjId )
	{
		parent = md->parent(parent);
		mdobjId = md->id(parent);
	}
	if ( context.isNull() )
	{
		aLog::print(aLog::Error,QObject::tr("aDataTable try set mdobject to null"));
		return;
	}
	mapCat.clear();
	mapDoc.clear();
	mapReg.clear();
	mapDim.clear();
	mapSum.clear();
	clearObjectHash( p_cat );
	clearObjectHash( p_reg );
	clearObjectHash( p_doc );

	// rebuild field list: real columns first, then virtual calculated fields
	m_schema = m_dbRecord;
	m_calculated.clear();

	if(md->objClass(context) == md_field && md->objClass(md->parent(context))== md_dimensions)
	{
		insertFieldInfo(context,false);
		aCfgItem res;
		aCfgItem ress = md->findChild(md->parent(md->parent(context)),md_resources);
		uint n = md->count( ress, md_field );
		for ( uint i = 0; i < n; i++ )
		{
			res = md->find( ress, md_field, i );
			insertFieldInfo(res,false);
		}
	}
	uint n = md->count( context, md_field );
	for ( uint i = 0; i < n; i++ )
	{
		cobj = md->find( context, md_field, i );
		insertFieldInfo(cobj);

	}
}


/*!
 *	\~english
 *	Appends info about field to object.
 *	\~russian
 *	Добавляет информацию о поле к объекту.
 *	\~
 */
void
aSQLTable::insertFieldInfo(aCfgItem cobj, bool calculated)
{
	QString fname, fdbname, objt;//, fid;
	int fid ;

	if ( !cobj.isNull() )
	{
		fid = md->id(cobj);
		fname = md->attr(cobj, mda_name);
		objt = md->attr( cobj, mda_type ).toUpper();
			fdbname = QString("uf%1").arg( fid );
                        if ( objt[0]=='O' )
			{
				fnames.insert( fname, fdbname );
        			fdbname = QString("text_uf%1").arg( fid );
                		append( QSqlField( fdbname, QVariant::String ) );
		                setCalculated( fdbname, calculated );
				int ftid = objt.section(" ", 1, 1 ).toInt();
				aCfgItem fto = md->find( ftid );
				if ( !fto.isNull() )
				{
					if ( md->objClass( fto ) == md_catalogue )
					{
						mapCat[fid] = fto;
					}
					if ( md->objClass( fto ) == md_document )
					{
						mapDoc[fid] = fto;
					}
				}
                        }
			else
                        if ( objt[0]==' ' )
			{
        			fdbname = QString("text_uf%1").arg( fid );
                		append( QSqlField( fdbname, QVariant::String ) );
		                setCalculated( fdbname, calculated );
				fnames.insert( fname, fdbname );
				int ftid = objt.section(" ", 1, 1 ).toInt();
				aCfgItem fto = md->find( ftid );
				if ( !fto.isNull() )
				{
					if ( md->objClass( fto ) == md_aregister )
					{
					aCfgItem s_field = md->find(objt.section(" ",2,2).toInt());
					aCfgItem dim_fields = md->find(fto,md_dimensions);

						if(!dim_fields.isNull())
						{
							int cnt = md->count( dim_fields, md_field );
							for ( int k = 0; k < cnt; k++  )
							{
								aCfgItem dim_field = md->find( dim_fields, md_field, k );
								QString type =  md->attr(dim_field, mda_type);
								if(type[0]=='O')
								{

									if(type.section(" ",1,1).toInt()== mdobjId)
									{
										mapReg[fid]=fto;
										mapDim[fid]= md->attr(dim_field,mda_name);
										mapSum[fid] = md->attr(s_field,mda_name);
										break;
									}
								}
							}
						}
					}
				}

                        }
			else
			{
				fnames.insert( fname, fdbname );
			}
	}
}


/*!
 *	Gets metadata object id.
 */
qulonglong
aDataTable::getIdd(void)
{
	return 0;
}
long
aDataTable::getMdObjId()
{
	return mdobjId;
}


/*! Stub. */
void
aDataTable::appendField( aSQLField * ) // fieldinfo )
{

}


/*! Stub. */
void
aDataTable::insertField( int /*pos*/, aSQLField * ) // fieldinfo )
{

}


/*! Stub. */
void
aDataTable::removeField( int ) // pos )
{

}


/*! Stub. */
void
aDataTable::clearFields()
{

}


/*!
 *	Check table structure.
 */
bool
aDataTable::checkStructure(  bool ) //update )
{
	bool rc = false;
	if ( name().isEmpty() ) {

	} else {

	}
	return rc;
}


/* ----------------------------------------------------------------------- */
/* schema                                                                   */
/* ----------------------------------------------------------------------- */

void
aDataTable::append( const QSqlField & field )
{
	if ( !m_schema.contains( field.name() ) )
		m_schema.append( field );
}

void
aDataTable::append( const QString & name, QVariant::Type type )
{
	append( QSqlField( name, type ) );
}

void
aDataTable::insert( int pos, const QSqlField & field )
{
	m_schema.insert( pos, field );
}

void
aDataTable::remove( int pos )
{
	m_schema.remove( pos );
}

void
aDataTable::clear()
{
	m_schema.clear();
	m_calculated.clear();
}

void
aDataTable::setGenerated( const QString &, bool )
{
	// no-op: generated fields are not supported by the QSqlQuery based model
}

void
aDataTable::setCalculated( const QString & name, bool calculated )
{
	m_calculated.insert( name, calculated );
}

bool
aDataTable::isCalculated( const QString & name ) const
{
	return m_calculated.value( name, false );
}

bool
aDataTable::contains( const QString & name ) const
{
	return m_schema.contains( name );
}

int
aDataTable::count() const
{
	return m_schema.count();
}

QString
aDataTable::fieldName( int i ) const
{
	return m_schema.fieldName( i );
}

QSqlField
aDataTable::field( int i ) const
{
	QSqlField f = m_schema.field( i );
	f.setValue( currentValue( f.name() ) );
	return f;
}

QSqlField
aDataTable::field( const QString & name ) const
{
	if ( !m_schema.contains( name ) )
		return QSqlField();
	QSqlField f = m_schema.field( name );
	f.setValue( currentValue( name ) );
	return f;
}

bool
aDataTable::canInsert() const
{
	return !m_readOnly;
}

QSqlIndex
aDataTable::primaryIndex( bool ) const
{
	QSqlIndex idx;
	if ( db ) {
		QSqlDatabase *d = db->db();
		if ( d && d->driver() )
			idx = d->driver()->primaryIndex( tableName );
	}
	return idx;
}


/* ----------------------------------------------------------------------- */
/* values                                                                   */
/* ----------------------------------------------------------------------- */

QVariant
aDataTable::currentValue( const QString & name ) const
{
	if ( m_current.contains( name ) )
		return m_current.value( name );
	return QVariant();
}

bool
aDataTable::isValid() const
{
	return m_index >= 0 && m_index < m_rows.size();
}

bool
aDataTable::isNull( int i ) const
{
	if ( !isValid() ) return true;
	return currentValue( fieldName( i ) ).isNull();
}

/*!
 *	Return field value by index.
 */
QVariant
aDataTable::value ( int i )
{
	return sysValue( fieldName( i ) );
}



/*!
 *	Return field value by metadata name.
 */
QVariant
aDataTable::value ( const QString & name )
{
	if ( !fnames.contains(name) )
	{
		aLog::print(aLog::Error, QObject::tr("aDataTable get value of unknown field `%1'").arg(name));
		return QVariant::Invalid;
	}
	return sysValue( fnames.value( name ) );
}



/*!
 *	Check field existing.
 */
bool
aDataTable::sysFieldExists( const QString & name )
{
	return contains( name );
}



/*!
 *	Return value of the column of the database table.
 */
QVariant
aDataTable::sysValue ( const QString & name )
{
	if(isCalculated(name)) return calcFieldValue(name);
	else return currentValue( name );
}



/*!
 *
 */
void
aDataTable::setSysValue ( const QString & name, QVariant value )
{
	if ( name == QString("pnum") )
	{
		aLog::print(aLog::Info, QObject::tr("aDataTable get document prefix to `%1'").arg(value.toString()));
	}
	if ( m_current.contains( name ) )
		m_current.setValue( name, value );
}



/*!
 * Set value of the column of the database table.
 */
void
aDataTable::setValue ( int i, QVariant value )
{
	setSysValue( fieldName( i ), value );
}



/*!
 *
 */
bool
aDataTable::setValue ( const QString & name, QVariant value )
{
	QString fname;
	if ( !fnames.contains(name) ) return false;
	fname = fnames.value( name );
	if ( contains( fname ) ) {
		setSysValue( fname, value );
	}
	else return false;
	return true;
}


/* ----------------------------------------------------------------------- */
/* edit buffer / CRUD                                                       */
/* ----------------------------------------------------------------------- */

QSqlRecord *
aDataTable::prepareInsertBuffer()
{
	m_editBuffer = m_dbRecord;
	for ( int i = 0; i < m_editBuffer.count(); i++ )
		m_editBuffer.setValue( i, QVariant() );
	return &m_editBuffer;
}

QSqlRecord *
aDataTable::prepareUpdateBuffer()
{
	m_editBuffer = m_current;
	return &m_editBuffer;
}

QSqlRecord *
aDataTable::primeInsert()
{
	return prepareInsertBuffer();
}

QSqlRecord *
aDataTable::primeUpdate()
{
	return prepareUpdateBuffer();
}

QSqlRecord *
aDataTable::primeDelete()
{
	return prepareUpdateBuffer();
}

QSqlRecord *
aDataTable::editBuffer( bool )
{
	return &m_editBuffer;
}

QSqlRecord *
aDataTable::insertBuffer()
{
	return prepareInsertBuffer();
}

QSqlRecord *
aDataTable::updateBuffer()
{
	return prepareUpdateBuffer();
}

QSqlRecord *
aDataTable::currentRecord()
{
	m_editBuffer = m_current;
	return &m_editBuffer;
}

int
aDataTable::insert()
{
	if ( m_readOnly || !db ) return 0;
	QStringList cols;
	QVariantList vals;
	for ( int i = 0; i < m_editBuffer.count(); i++ ) {
		QString fn = m_editBuffer.fieldName( i );
		if ( !m_dbRecord.contains( fn ) ) continue;
		cols << fn;
		vals << m_editBuffer.value( i );
	}
	if ( cols.isEmpty() ) return 0;
	QStringList ph;
	for ( int i = 0; i < cols.size(); i++ ) ph << "?";
	QString sql = "INSERT INTO " + tableName + " (" + cols.join( ", " ) + ") VALUES (" + ph.join( ", " ) + ")";
	QSqlQuery q( *db->db() );
	q.prepare( sql );
	for ( int i = 0; i < vals.size(); i++ )
		q.bindValue( i, vals[i] );
	if ( !q.exec() ) {
		m_lastError = q.lastError();
		aLog::print(aLog::Error, QObject::tr("aDataTable insert error: %1").arg(m_lastError.text()));
		return 0;
	}
	return 1;
}

int
aDataTable::update()
{
	if ( m_readOnly || !db ) return 0;
	QSqlIndex pk = primaryIndex();
	QString keyName = ( pk.count() > 0 ) ? pk.fieldName( 0 ) : QString( "id" );
	QVariant keyVal = m_editBuffer.value( keyName );
	if ( !keyVal.isValid() ) keyVal = m_current.value( keyName );
	QStringList sets;
	for ( int i = 0; i < m_editBuffer.count(); i++ ) {
		QString fn = m_editBuffer.fieldName( i );
		if ( fn == keyName ) continue;
		if ( !m_dbRecord.contains( fn ) ) continue;
		sets << fn + "=?";
	}
	if ( sets.isEmpty() ) return 0;
	QString sql = "UPDATE " + tableName + " SET " + sets.join( ", " ) + " WHERE " + keyName + "=?";
	QSqlQuery q( *db->db() );
	q.prepare( sql );
	int b = 0;
	for ( int i = 0; i < m_editBuffer.count(); i++ ) {
		QString fn = m_editBuffer.fieldName( i );
		if ( fn == keyName ) continue;
		if ( !m_dbRecord.contains( fn ) ) continue;
		q.bindValue( b++, m_editBuffer.value( i ) );
	}
	q.bindValue( b, keyVal );
	if ( !q.exec() ) {
		m_lastError = q.lastError();
		aLog::print(aLog::Error, QObject::tr("aDataTable update error: %1").arg(m_lastError.text()));
		return 0;
	}
	return 1;
}

int
aDataTable::del()
{
	if ( m_readOnly || !db ) return 0;
	QSqlIndex pk = primaryIndex();
	QString keyName = ( pk.count() > 0 ) ? pk.fieldName( 0 ) : QString( "id" );
	QVariant keyVal = m_editBuffer.value( keyName );
	if ( !keyVal.isValid() ) keyVal = m_current.value( keyName );
	QSqlQuery q( *db->db() );
	q.prepare( "DELETE FROM " + tableName + " WHERE " + keyName + "=?" );
	q.bindValue( 0, keyVal );
	if ( !q.exec() ) {
		m_lastError = q.lastError();
		aLog::print(aLog::Error, QObject::tr("aDataTable delete error: %1").arg(m_lastError.text()));
		return 0;
	}
	return 1;
}


/* ----------------------------------------------------------------------- */
/* select / filter                                                          */
/* ----------------------------------------------------------------------- */

bool
aDataTable::doSelect( const QString & where )
{
	if ( !db ) return false;
	QString sql = "SELECT * FROM " + tableName;
	if ( !where.isEmpty() ) sql += " WHERE " + where;
	QSqlQuery q( *db->db() );
	if ( !q.exec( sql ) ) {
		m_lastError = q.lastError();
		aLog::print(aLog::Error, QObject::tr("aDataTable select error: %1").arg(m_lastError.text()));
		return false;
	}
	m_rows.clear();
	while ( q.next() )
		m_rows.append( q.record() );
	m_index = -1;
	m_current.clear();
	selected = true;
	return true;
}

bool
aDataTable::select( const QString & filter, bool usefltr )
{
	QString flt;
	if ( usefltr )
	{
		flt = getFilter();
		if ( flt.isEmpty() ) flt = m_filter;
		else if ( !m_filter.isEmpty() ) flt = m_filter + " AND " + flt;
		if ( !filter.isEmpty() )
			flt = flt.isEmpty() ? filter : flt + " AND " + filter;
	}
	else flt = filter;
	return doSelect( flt );
}



bool
aDataTable::select( qulonglong id )
{
	return doSelect( QString( "id=%1" ).arg( id ) );
}



void
aDataTable::clearFilter()
{
	aLog::print(aLog::Debug, QObject::tr("aDataTable clear filter"));
	userFilter.clear();
}



void
aDataTable::setFilter ( const QString & newFilter ) {
	m_filter = newFilter;
}



bool
aDataTable::setFilter( const QString& name, const QVariant& value )
{
	aLog::print(aLog::Debug, QObject::tr("aDataTable set filter %1='%2'").arg(name).arg(value.toString()));
	if ( !fnames.contains(name) )
	{
		aLog::print(aLog::Error, QObject::tr("aDataTable set filter %1='%2', %3 not exist").arg(name).arg(value.toString()).arg(name));
		return false;
	}
	userFilter.insert( fnames.value(name), value );
	return true;
}



QString
aDataTable::getFilter()
{
	QString filter = "", fid, type;
	aCfgItem field;
	QHashIterator<QString, QVariant> it( userFilter );
	bool first = true;
	while ( it.hasNext() )
	{
		it.next();
		fid = it.key().mid(2);
		field = md->find(fid.toLong() );
		if ( !field.isNull() )
		{
			type = md->attr( field, mda_type );
			if ( first )
			{
				if ( type[0] == 'N' || type[0] == 'O' )
					filter = it.key() + "=" + it.value().toString();
				else
					filter = it.key() + "='" + it.value().toString() + "'";
				first = false;
			}
			else
			{
				if ( type[0] == 'N' || type[0] == 'O' )
					filter += " and " + it.key() + "=" + it.value().toString();
				else
					filter += " and " + it.key() + "='" + it.value().toString() + "'";
			}
		}
	}
	return filter;
}



QString
aDataTable::getNFilter()
{
	QString filter = "", fid, type;
	aCfgItem field;
	QHashIterator<QString, QVariant> it( userFilter );
	bool first = true;
	while ( it.hasNext() )
	{
		it.next();
		fid = it.key().mid(2);
		field = md->find(fid.toLong() );
		if ( !field.isNull() )
		{
			type = md->attr( field, mda_type );
			if ( first )
			{
				if ( type[0] == 'N' || type[0] == 'O' )
					filter = tableName + "." + it.key() + "=" + it.value().toString();
				else
					filter = tableName + "." + it.key() + "='" + it.value().toString() + "'";
				first = false;
			}
			else
			{
				if ( type[0] == 'N' || type[0] == 'O' )
					filter += " and " + tableName + "." + it.key() + "=" + it.value().toString();
				else
					filter += " and " + tableName + "." + it.key() + "='" + it.value().toString() + "'";
			}
		}
	}
	return filter;
}



/*!
 *	Print current record.
 */
void
aDataTable::printRecord(){
	unsigned int i;
	QString fname, sname;

	for (i=0; i< (unsigned int) count(); i++){
		fname = fieldName( i );
		sname = fname;
		printf("%s(%s)=%s\n",
		( const char *) fname.toLocal8Bit(),
		( const char *) sname.toLocal8Bit(),
		( const char *) value( i ).toString().toLocal8Bit() );
	}
}



bool
aDataTable::exec( QString query )
{
	if ( !db ) return false;
	QSqlQuery q( *db->db() );
	if ( !q.exec( query ) ) {
		m_lastError = q.lastError();
		return false;
	}
	if ( q.isSelect() ) {
		m_rows.clear();
		while ( q.next() )
			m_rows.append( q.record() );
		m_index = -1;
		m_current.clear();
		selected = true;
	}
	return true;
}



QVariant
aDataTable::calc_rem(int fid, qulonglong id)
{
	aCfgItem o,fto;
	QString t,oclass;
	int ftid,oid;
	QVariant v="";
	fto = mapReg[fid];// = md->find( ftid );
	if ( !fto.isNull() )
	{
		if(p_reg[QString("%1").arg(fid)]==0)
		{
			p_reg.insert(QString("%1").arg(fid), new aARegister( fto, db ));
		}
		v = ((aARegister*)p_reg[QString("%1").arg(fid)])->getSaldo( QDateTime::currentDateTime(),
					mapDim[fid],
					id,
					mapSum[fid]);//md->attr(s_field,mda_name));
		if(!v.isValid()) v = "";
	}
	return v;
}


QVariant
aDataTable::calc_obj(int fid, qulonglong idd)
{
	aCfgItem o,fto;
	QString t,oclass;
	int ftid;
	QVariant v="";
	o = mapCat[fid];
	if( !o.isNull() )
	{
		aCatalogue *pCat = (aCatalogue*)p_cat[QString("%1").arg(fid)];
		if(pCat==0)
		{
			p_cat.insert(QString("%1").arg(fid), new aCatalogue( o, db ));
			pCat = (aCatalogue*)p_cat[QString("%1").arg(fid)];
		}
		pCat->select( idd );

		if ( pCat->selected() )
		{
			v = QVariant( pCat->displayString() );
		}
	}
	else
	{
		o = mapDoc[fid];
		if(!o.isNull())
		{
			aDocument *pDoc = (aDocument*)p_doc[QString("%1").arg(fid)];
			if(pDoc==0)
			{
				p_doc.insert(QString("%1").arg(fid), new aDocument( o, db ));
				pDoc = (aDocument*)p_doc[QString("%1").arg(fid)];
			}
			pDoc->select( idd );
			if ( pDoc->selected() )
			{
				v = QVariant( pDoc->displayString() );
			}
		}
	}

	return v;
}

/*!
 * Calculate value object type fields.
 */
QVariant
aDataTable::calculateField( const QString &name )
{
	QVariant v;
	if(name=="system_icon") return v;
	else return QVariant("");
//	return QVariant::Invalid;
}

/*!
 * Calculate value object type fields.
 */
QVariant
aDataTable::calcFieldValue( const QString &name )
{

        QVariant v="", fv=0;
	int fid = 0; //, ftid;
	qulonglong id = 0;
	aCfgItem o, fto;
	QString t, oclass;
        if ( name.left(5)=="text_" )
	{
                if(sysFieldExists(name.mid(5)))
		{
			fv = sysValue( name.mid( 5 ) );
			id = fv.toULongLong();
		}
		else
		{
			id = 0;
		}
		fid = name.mid( 7 ).toInt();
		v = QVariant("");
		if ( fid && id )
		{
			v=calc_obj(fid,id);
		}
		else
		{
			if(fid)
			{
				id = sysValue("id").toULongLong();
				v=calc_rem(fid,id);
			}
		}
        }
        return v;
}



/*!
 * Gets list of user and calculation fields.
 */
QStringList
aDataTable::getUserFields()
{
   QStringList lst;
   int i;
   const QString text_uf = "text_uf";
	for(i=0; i<count(); i++)
	{
		if(fieldName(i).left(2) == "uf"
		   || fieldName(i).left(text_uf.length()) == text_uf)
		{
			if(fieldName(i).left(text_uf.length()) == text_uf)
			{
				QString key = "uf"+fieldName(i).mid(text_uf.length());
				if(lst.contains(key))
				{
					lst.removeAll(key);
				}
			}
			lst << fieldName(i);
		}
	}
   return lst;
}



/*!
 *
 */
bool
aDataTable::isMarkDeleted()
{
        if ( sysFieldExists( "df" ) ) return sysValue( "df" ).toInt() == 1;
        return false;
}



/*!
 *
 */
ERR_Code
aDataTable::setMarkDeleted( bool Deleted )
{
        if ( sysFieldExists( "df" ) ) {
                QString v = "";
                if ( Deleted ) v = "1";
                setSysValue( "df", QVariant( v ) );
                return err_noerror;
        }
        return err_incorrecttype; // Object can not be mark deleted
}


/* ----------------------------------------------------------------------- */
/* navigation                                                               */
/* ----------------------------------------------------------------------- */

void
aDataTable::loadCurrent()
{
	if ( isValid() )
		m_current = m_rows.at( m_index );
	else
		m_current.clear();
}

bool
aDataTable::seek ( int i, bool relative )
{
	int idx = relative ? m_index + i : i;
	if ( idx >= 0 && idx < m_rows.size() ) {
		m_index = idx;
		loadCurrent();
		return true;
	}
	return false;
}

bool
aDataTable::next ()
{
	if ( m_index + 1 < m_rows.size() ) {
		++m_index;
		loadCurrent();
		return true;
	}
	return false;
}

bool
aDataTable::prev ()
{
	if ( m_index - 1 >= 0 ) {
		--m_index;
		loadCurrent();
		return true;
	}
	return false;
}

bool
aDataTable::first ()
{
	if ( m_rows.isEmpty() ) return false;
	m_index = 0;
	loadCurrent();
	return true;
}


bool
aDataTable::last ()
{
	if ( m_rows.isEmpty() ) return false;
	m_index = m_rows.size() - 1;
	loadCurrent();
	return true;
}



bool
aDataTable::New()
{
	qulonglong Uid = 0;
	bool res = false;

	prepareInsertBuffer();
	if ( sysFieldExists("id") )
	{
		Uid = db->uid( mdobjId );
		aLog::print(aLog::Debug, QString("aDataTable new record with id=%1 for meta object with id=%2").arg(Uid).arg(mdobjId));

		m_editBuffer.setValue("id", QVariant( Uid ) );
	}
	if ( insert() )
	{
		if ( select(QString("id=%1").arg(Uid), false) )
			if ( first() )
			{
				res = true;
			}
			else
			{
				aLog::print(aLog::Error, QString("aDataTable record with id=%1 not found").arg(Uid));
			}
	}
	return res;
}


bool
aDataTable::Copy()
{
	QSqlRecord rec = m_current;
	if ( New() )
	{
		for ( int i=0; i<rec.count(); i++ )
		{
			QString fn = rec.fieldName( i );
			if ( fn == QString("id") ) continue;
			if ( m_current.contains( fn ) )
				m_current.setValue( fn, rec.value( i ) );
		}
	}
	return Update();
}


bool
aDataTable::Delete()
{
	prepareUpdateBuffer();
	del();
	return true;
}


bool
aDataTable::Update()
{
	prepareUpdateBuffer();
	update();
	if ( m_lastError.type() != QSqlError::NoError )
	{
		aLog::print(aLog::Error, QString("%1 %2").arg(m_lastError.text()).arg(m_lastError.driverText()) );
	}
	return true;
}

/**
 */
QString
aDataTable::sqlFieldName ( const QString & userFieldName ) const {
	if ( fnames.contains(userFieldName) ) {
		return fnames.value( userFieldName );
	}
	return QString("");
}
