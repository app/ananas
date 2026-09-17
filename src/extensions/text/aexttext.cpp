/****************************************************************************
** $Id: aexttext.cpp,v 1.1 2008/11/05 21:16:28 leader Exp $
**
** Extension object implementation file of
** Ananas application library
**
** Created : 20031201
**
** Copyright (C) 2003-2006 Leader InfoTech.  All rights reserved.
** Copyright (C) 2003-2006 Valery Grazdankin, Orenburg.
** Copyright (C) 2003-2005 Grigory Panov, Yoshkar-Ola.
**
** This file is part of the Designer application of the Ananas
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

#include <QTextStream>
#include "aexttext.h"
#include "acfg.h"
#if QT_VERSION >= 0x060000
#include <QStringConverter>
#else
#include <QTextCodec>
#endif

/*!
\en
    \class AExtText aexttext.h
    \brief The AExtText is Ananas extension class for work with text files.
\_en \ru
    \class AExtText aexttext.h
    \brief Класс расширения Ананас AExtText предназначен для работы с текстовыми файлами.

    Поддерживает работу с файлами в различных кодировках,
    режимах:
    RO - Только чтение
    WO - Только запись
    RW - чтение/запись
    A  - добавление к концу файла.
\_ru
*/


// \param cmd (in) command text.
// \return - result.


/*!
 *\en
 *
 *\_en \ru
 *
 *\_ru
 */
AExtText::AExtText()
    : AExtension("Text")
{
	text = new QTextStream( &file );
	setCodec("UTF-8");
}



/*!
 *\en
 *
 *\_en \ru
 *
 *\_ru
 */
AExtText::~AExtText()
{
	delete text;
}


/*!
 *\en
 *
 *\_en \ru
 *
 *\_ru
 */
int
AExtText::open( const QString &filename, const QString &mode )
{
	QIODevice::OpenMode m = QIODevice::ReadOnly;
	file.close();
	file.setFileName( filename );
	if ( mode == "WO" ) m = QIODevice::WriteOnly;
	if ( mode == "RW" ) m = QIODevice::ReadWrite;
	if ( mode == "A" ) m = QIODevice::Append;
	if ( file.open( m ) ) return 0;
	return 1;
}


/*!
 *\en
 *
 *\_en \ru
 *
 *\_ru
 */
void
AExtText::close()
{
	file.close();
}


/*!
 *\en
 *
 *\_en \ru
 *
 *\_ru
 */
QString
AExtText::readLine()
{
	return text->readLine();
}



/*!
 *\en
 *
 *\_en \ru
 *
 *\_ru
 */
void
AExtText::writeLine( const QString &l )
{
	*text << l << "\n";
}



/*!
 *\en
 *
 *\_en \ru
 *
 *\_ru
 */
bool
AExtText::atEnd()
{
	return text->atEnd();
}



/*!
 *\en
 *
 *\_en \ru
 *
 *\_ru
 */
QString
AExtText::getCodec() const
{
#if QT_VERSION >= 0x060000
	switch ( text->encoding() ) {
	case QStringConverter::Utf8:    return "UTF-8";
	case QStringConverter::Utf16:   return "UTF-16";
	case QStringConverter::Utf16LE: return "UTF-16LE";
	case QStringConverter::Utf16BE: return "UTF-16BE";
	case QStringConverter::Utf32:   return "UTF-32";
	case QStringConverter::Utf32LE: return "UTF-32LE";
	case QStringConverter::Utf32BE: return "UTF-32BE";
	case QStringConverter::Latin1:  return "ISO-8859-1";
	case QStringConverter::System:  return "System";
	default:                        return "UTF-8";
	}
#else
	QTextCodec *codec = text->codec();
	if ( codec ) return codec->name();
	return "";
#endif
}



/*!
 *\en
 *
 *\_en \ru
 *
 *\_ru
 */
void
AExtText::setCodec( const QString &codecname )
{
#if QT_VERSION >= 0x060000
	const QString n = codecname.toUpper();
	if ( n == "UTF-8" || n == "UTF8" ) text->setEncoding( QStringConverter::Utf8 );
	else if ( n == "UTF-16" ) text->setEncoding( QStringConverter::Utf16 );
	else if ( n == "UTF-16LE" ) text->setEncoding( QStringConverter::Utf16LE );
	else if ( n == "UTF-16BE" ) text->setEncoding( QStringConverter::Utf16BE );
	else if ( n == "UTF-32" ) text->setEncoding( QStringConverter::Utf32 );
	else if ( n == "UTF-32LE" ) text->setEncoding( QStringConverter::Utf32LE );
	else if ( n == "UTF-32BE" ) text->setEncoding( QStringConverter::Utf32BE );
	else if ( n == "ISO-8859-1" || n == "LATIN1" ) text->setEncoding( QStringConverter::Latin1 );
	else text->setEncoding( QStringConverter::Utf8 );
#else
	QTextCodec *codec = QTextCodec::codecForName( codecname.toLatin1().constData() );
	if ( codec ) text->setCodec( codec );
#endif
}


#include <aextensionplugin.h>

class AExtTextPlugin : public AExtensionPlugin<AExtText>
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Ananas.Extensions.AExtensionInterface/1.0")
public:
    AExtTextPlugin() {}
};

#include "aexttext.moc"
