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
#ifndef SYSUID_H
#define SYSUID_H

#include <QObject>
#include <QPointer>
#include "mnamespace.h"
#include <QSharedPointer>
#include "applicationcontext.h"

#ifdef HAVE_QMSYSTEM
#include <qmlocks.h>
#endif

class UsbUi;
class BatteryBusinessLogic;
class ShutdownBusinessLogic;
class StatusAreaRenderer;
class StatusIndicatorMenuBusinessLogic;
class ScreenLockBusinessLogic;
class VolumeBarLogic;
class MApplicationExtensionArea;

class Sysuid : public QObject
{
    Q_OBJECT

public:
    Sysuid(QObject* parent);
    virtual ~Sysuid();
    static Sysuid* instance();

signals:
    /*!
      * Inform about orientation changes
      */
    void orientationChangeFinished(const M::Orientation &);

private:
    void loadTranslations();

private:
    BatteryBusinessLogic *batteryBusinessLogic;
    ShutdownBusinessLogic *shutdownBusinessLogic;
    UsbUi *usbUi;
    static Sysuid *instance_;

    //! Status area
    StatusAreaRenderer *statusAreaRenderer;

    //! Status indicator menu
    StatusIndicatorMenuBusinessLogic *statusIndicatorMenuBusinessLogic;

    //! The lock screen business logic
    ScreenLockBusinessLogic *screenLockBusinessLogic;

    //! The volume extension area
    MApplicationExtensionArea *volumeExtensionArea;

#ifdef HAVE_QMSYSTEM
    //! QmSystem watcher for device lock
    MeeGo::QmLocks qmLocks;
#endif

#ifdef UNIT_TEST
    friend class Ut_Sysuid;
#endif
};

#endif // SYSUID_H
