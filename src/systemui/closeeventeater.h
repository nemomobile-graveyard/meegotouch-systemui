/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of systemui.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef CLOSEEVENTEATER_H_
#define CLOSEEVENTEATER_H_

#include <QObject>

/*!
 * Close event eater is an object that "eats" QCloseEvents by accepting them.
 * It can be installed to windows so that they will not react to CTRL-Q
 * presses.
 */
class CloseEventEater : public QObject
{
    Q_OBJECT

public:
    /*!
     * Creates a close event eater.
     *
     * \param parent the parent object
     */
    CloseEventEater(QObject *parent = NULL);

protected:
    //! \reimp
    bool eventFilter(QObject *obj, QEvent *event);
    //! \reimp_end
};

#endif /* CLOSEEVENTEATER_H_ */