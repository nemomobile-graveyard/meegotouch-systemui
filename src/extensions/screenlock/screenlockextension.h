/****************************************************************************
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

#ifndef SCREENLOCKEXTENSION_H
#define SCREENLOCKEXTENSION_H

#include "screenlockextensioninterface.h"
#include <QObject>

class LockScreen;

class ScreenLockExtension : public QObject, public ScreenLockExtensionInterface
{
    Q_OBJECT
    Q_INTERFACES(ScreenLockExtensionInterface MApplicationExtensionInterface)

public:
    ScreenLockExtension();
    virtual ~ScreenLockExtension();

    // Methods derived from ScreenLockExtensionInterface
    virtual void reset();
    virtual void setNotificationManagerInterface(NotificationManagerInterface &notificationManager);

    // Methods derived from MApplicationExtensionInterface
    virtual bool initialize(const QString &interface);
    virtual QGraphicsWidget *widget();

    // Returns the notification manager interface
    static NotificationManagerInterface *notificationManagerInterface();
private:
    //! The lock screen widget
    LockScreen *lockScreen;

    //! Notification manager to use
    static NotificationManagerInterface *notificationManager;
};

#endif // SCREENLOCKEXTENSION_H
