/****************************************************************************
** $Id: formdesigner.h,v 1.3 2008/11/27 20:20:55 leader Exp $
**
** Header file of the Form designer of Ananas Designer applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
**
** This file is part of the Designer application  of the Ananas
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

#ifndef FORMDESIGNER_H
#define FORMDESIGNER_H

#include "designer6/aworkbench.h"

// Facade kept for the metadata editor, which only opens/saves forms and
// shows/hides the designer window.
class aFormDesigner : public aDesignerWorkbench
{
    Q_OBJECT

public:
    aFormDesigner();
    ~aFormDesigner() override;

    void fileOpen(const QString &fileName);
    void fileSaveAll();
};

#endif // FORMDESIGNER_H
