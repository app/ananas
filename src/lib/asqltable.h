/****************************************************************************
** $Id: asqltable.h,v 1.2 2009/05/25 16:35:10 app Exp $
**
** Header file of the Ananas database table of Ananas
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

#ifndef ASQLTABLE_H
#define ASQLTABLE_H

#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlError>
#include <QList>
#include <QHash>
#include <QStringList>

#include "ananasglobal.h"
#include "acfg.h"

// temporary define for old definitions
#define aSQLTable aDataTable

class aDatabase;
class aSQLField;

/*!
 *	\~english
 *	Provides browsing and editing Ananas's sql tables mantained by Ananas.Designer.
 *	\~russian
 *	\brief Определяет программный интерфейс модели данных aDataTable.
 *
 *	Позволяет работать с табличными представлениями данных, определенных метаданными бизнес схемы.
 *
 *	Реализован поверх QSqlQuery/QSqlRecord: выборка буферизуется в память,
 *	навигация выполняется по индексу, запись/обновление/удаление — через
 *	подготовленные запросы. Больше не наследует Qt-курсор.
 *	\~
 */
class  ANANAS_EXPORT aDataTable
{
public:
	aDatabase*	db;
	QString 	tableName;
	bool 		selected;

				aSQLTable( aCfgItem context, aDatabase *adb );
				aSQLTable( const QString &tname, aDatabase *adb );
	virtual 	~aSQLTable();
	void init( aCfgItem context, aDatabase *adb );
	void setObject(aCfgItem context);
	void appendField( aSQLField * fieldinfo );
	void insertField( int pos, aSQLField * fieldinfo );
	void removeField( int pos );
	void clearFields();
	bool checkStructure( bool update );
	long getMdObjId();
	qulonglong getIdd();
	void		printRecord();

	/* schema */
	void		append( const QSqlField & field );
	void		append( const QString & name, QVariant::Type type = QVariant::Invalid );
	void		insert( int pos, const QSqlField & field );
	void		remove( int pos );
	void		clear();
	void		setGenerated( const QString & name, bool generated ) ANANAS_DEPRECATED;
	void		setCalculated( const QString & name, bool calculated );
	bool		isCalculated( const QString & name ) const;
	bool		contains( const QString & name ) const;
	int		count() const;
	QString		fieldName( int i ) const;
	QSqlField	field( int i ) const;
	QSqlField	field( const QString & name ) const;
	QString		name() const { return tableName; }
	int		position( const QString & name ) const { return m_schema.indexOf( name ); }
	QString		filter() const { return m_filter; }
	bool		isReadOnly() const { return m_readOnly; }
	void		setReadOnly( bool ro ) { m_readOnly = ro; }
	bool		canInsert() const;
	bool		canUpdate() const { return canInsert(); }
	bool		canDelete() const { return canInsert(); }
	QSqlIndex	primaryIndex( bool prime = true ) const;
	QSqlError	lastError() const { return m_lastError; }

	virtual QVariant value ( int i );
	virtual QVariant value ( const QString & name );
	virtual void setValue ( int i, QVariant value );
	virtual bool setValue ( const QString & name, QVariant value );
	virtual QVariant sysValue ( const QString & name );
	virtual void setSysValue ( const QString & name, QVariant value );
	virtual bool sysFieldExists( const QString & name );

	virtual QSqlRecord *primeInsert() ANANAS_DEPRECATED;
	virtual QSqlRecord *primeUpdate() ANANAS_DEPRECATED;
	virtual QSqlRecord *primeDelete() ANANAS_DEPRECATED;
	QSqlRecord	*insertBuffer();
	QSqlRecord	*updateBuffer();
	QSqlRecord	*currentRecord();
	virtual QSqlRecord *editBuffer( bool copy = false );
	virtual int insert();
	virtual int update();
	virtual int del();

	virtual bool select( const QString & filter="", bool usefltr = true );
	virtual bool select( qulonglong id );
	virtual void		clearFilter();
	virtual bool		setFilter( const QString &name, const QVariant &value );
	virtual void		setFilter( const QString& );
	virtual QString		getFilter();
	virtual QString		getNFilter();
	virtual bool		exec( QString query ) ANANAS_DEPRECATED;
	virtual QStringList	getUserFields();
	virtual ERR_Code 	setMarkDeleted( bool Deleted );
	virtual bool 		isMarkDeleted();
	QString 		sqlFieldName ( const QString & userFieldName ) const;

	virtual bool New();
	virtual bool Copy();
	virtual bool Delete();
	virtual bool Update();

	virtual bool seek ( int i, bool relative = FALSE );
	virtual bool next ();
	virtual bool prev ();
	virtual bool first ();
	virtual bool last ();
	int		at() const { return m_index; }
	int		size() const { return m_rows.size(); }
	bool		isValid() const;
	bool		isNull( int i ) const;

protected:
	QVariant calcFieldValue( const QString &name );
	virtual QVariant calculateField( const QString &name );
	virtual QVariant calc_obj(int fid,qulonglong idd);
	virtual QVariant calc_rem(int fid,qulonglong id);
	void insertFieldInfo(aCfgItem cobj, bool calculated=true);

	long		mdobjId;

private:
	bool		doSelect( const QString & where );
	void		loadCurrent();
	QVariant	currentValue( const QString & name ) const;
	QSqlRecord	*prepareInsertBuffer();
	QSqlRecord	*prepareUpdateBuffer();

	aCfgItem		obj;
	aCfgItem		init_obj;
	aCfg*			md;
	QHash<QString, QObject*> p_cat;
	QHash<QString, QObject*> p_doc;
	QHash<QString, QObject*> p_reg;
	QMap<int,aCfgItem> mapCat, mapReg, mapDoc;
	QMap<int,QString> mapDim,mapSum;
	QStringList fildsList;
	QHash<QString, QString> fnames;
	QHash<QString, QVariant> userFilter;

	QSqlRecord		m_schema;	// field list (real columns + virtual calculated fields)
	QHash<QString, bool>	m_calculated;
	QSqlRecord		m_dbRecord;	// real table columns (driver record)
	QSqlRecord		m_current;	// current row values (editable)
	QSqlRecord		m_editBuffer;	// buffer for insert/update/delete
	QList<QSqlRecord>	m_rows;		// buffered result set
	int			m_index;
	QString			m_filter;
	QSqlError		m_lastError;
	bool			m_readOnly;
};

#endif
