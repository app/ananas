/****************************************************************************
** $Id: qdataschemaglobal.h,v 1.16 2008/11/09 20:03:16 leader Exp $
**
** QT Data Schema project
**
** Copyright (C) 1999-2006 Leader InfoTech.  All rights reserved.
** Copyright (C) 1999-2006 Valery Grazdankin.  All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.leaderit.ru/ or email sales@leaderit.ru
**
**********************************************************************/
#ifndef QDATASCHEMAGLOBAL_H
#define QDATASCHEMAGLOBAL_H
#define QDS_VERSION 0x120
#include <qglobal.h>

#define DEBUG

#include <QtCore>
#include <QtSql>
#include <QtXml>
#include <QCoreApplication>
#include <QStringList>
#define QSqlFieldInfo QSqlField

#define QDS_SQLTable       QSqlTableModel
#define QDS_int            int
#define QDS_IO_ReadOnly    QIODevice::ReadOnly
#define QDS_IO_WriteOnly   QIODevice::WriteOnly
#define QTextStream_readAll( t )    t.readAll()

#ifdef Q_OS_WIN
#	include <windows.h>
#	include <math.h>
#endif
#ifdef Q_OS_UNIX
#	include <unistd.h>
#endif

#if defined(Q_OS_WIN) && !defined(LIB_NO_DLL)
#  define LIB_DLLIMPORT __declspec(dllimport)
#  define LIB_DLLEXPORT __declspec(dllexport)
#else
#  define LIB_DLLIMPORT
#  define LIB_DLLEXPORT
#endif

#if defined (Q_CC_MSVC)
#define TEMPLATE_EXTERN extern
#else
#define TEMPLATE_EXTERN
#endif

#ifdef QDATASCHEMA_EXPORT
#  define LIB_EXPORT LIB_DLLEXPORT
#else
#  define LIB_EXPORT LIB_DLLIMPORT
#endif

#ifdef QDATASCHEMA_EXPORT
#  define LIB_TEMPLATE_EXTERN
#else
#  define LIB_TEMPLATE_EXTERN TEMPLATE_EXTERN
#endif

#if defined(Q_CC_MSVC)
#  define vsnprintf _vsnprintf
#endif


class QDS
{
public:
/*!
 * Define Meta Object types.
 */
    enum MO_Type { MO_ROOT, MO_CLASS, MO_ATTR, MO_RELATION };

    static int version();
};


QChar   toLower(QChar c);
QString toLower(const QString &s);
char    toAscii(QChar c);
const char*   toChar(QString str);
int     indexOf( const QString &str, const QString &find_str );
const char *toLocal8Bit(const QString &s);
QStringList split(const QString &div, const QString &str );
QString trimm(const QString &s);
#endif
