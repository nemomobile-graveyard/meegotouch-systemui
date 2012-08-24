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
#include <QTimer>
#include <QDBusInterface>
#include <QTextStream>
#include <QDebug>
#include <QCursor>

#include "screenlockbusinesslogic.h"
#include "notificationstatusindicatorsink.h"
#include "eventeater.h"
#include "closeeventeater.h"
#include "sysuid.h"

ScreenLockBusinessLogic::ScreenLockBusinessLogic(QObject* parent) :
    QObject(parent),
    eventEaterWindow(NULL),
    callbackInterface(NULL),
    shuttingDown(false),
    lockscreenVisible(false)
{
#ifdef HAVE_QMSYSTEM
    connect(&systemState, SIGNAL(systemStateChanged(MeeGo::QmSystemState::StateIndication)), this, SLOT(systemStateChanged(MeeGo::QmSystemState::StateIndication)));
#endif

    // when lipstick tells us the screen is unlocked, tell mce that the screen is unlocked.
    QDBusConnection::sessionBus().connect("org.nemomobile.lipstick", "/request", "org.nemomobile.lipstick", "screenUnlocked", this, SLOT(unlockScreen()));
}

ScreenLockBusinessLogic::~ScreenLockBusinessLogic()
{
    delete eventEaterWindow;
}

int ScreenLockBusinessLogic::tklock_open(const QString &service, const QString &path, const QString &interface, const QString &method, uint mode, bool, bool)
{
    if (shuttingDown) {
        // Don't show the touch screen lock while shutting down
        return TkLockReplyOk;
    }

    // Create a D-Bus interface if one doesn't exist or the D-Bus callback details have changed
    if (callbackInterface == NULL || callbackInterface->service() != service || callbackInterface->path() != path || callbackInterface->interface() != interface) {
        delete callbackInterface;
        callbackInterface = new QDBusInterface(service, path, interface, QDBusConnection::systemBus(), this);
    }

    // Store the callback method name
    callbackMethod = method;

    // MCE needs a response ASAP, so the actions are executed with single shot timers
    switch (mode) {
    case TkLockModeEnable:
        // Create the lock screen already so that it's readily available
        QTimer::singleShot(0, this, SLOT(showScreenLock()));
        break;

    case TkLockModeOneInput:
        QTimer::singleShot(0, this, SLOT(showEventEater()));
        break;

    case TkLockEnableVisual:
        // Raise the lock screen window on top if it isn't already
        QTimer::singleShot(0, this, SLOT(showScreenLock()));
        break;

    case TkLockEnableLowPowerMode:
        // Enable low power mode and raise the lock screen window on top if it isn't already
        QTimer::singleShot(0, this, SLOT(showLowPowerMode()));
        break;

    case TkLockRealBlankMode:
        QTimer::singleShot(0, this, SLOT(setDisplayOffMode()));
        break;

    default:
        break;
    }

    return TkLockReplyOk;
}

int ScreenLockBusinessLogic::tklock_close(bool)
{
    QTimer::singleShot(0, this, SLOT(hideScreenLockAndEventEater()));

    return TkLockReplyOk;
}

void ScreenLockBusinessLogic::unlockScreen()
{
    toggleScreenLockUI(false);
    toggleEventEater(false);

    if (callbackInterface != NULL && !callbackMethod.isEmpty())
        callbackInterface->call(QDBus::NoBlock, callbackMethod, TkLockUnlock);
}

void ScreenLockBusinessLogic::showScreenLock()
{
    toggleScreenLockUI(true);
    toggleEventEater(false);
}

void ScreenLockBusinessLogic::showLowPowerMode()
{
    toggleScreenLockUI(true);
    toggleEventEater(false);
    qWarning() << Q_FUNC_INFO << "We don't support LPM.";
}

void ScreenLockBusinessLogic::setDisplayOffMode()
{
    toggleScreenLockUI(true);
    toggleEventEater(false);
}

void ScreenLockBusinessLogic::hideScreenLockAndEventEater()
{
    toggleScreenLockUI(false);
    toggleEventEater(false);
}

void ScreenLockBusinessLogic::showEventEater()
{
    toggleEventEater(true);
}

void ScreenLockBusinessLogic::hideEventEater()
{
    toggleEventEater(false);
}

void ScreenLockBusinessLogic::toggleScreenLockUI(bool toggle)
{
    const QDBusConnection &sessionBus = QDBusConnection::sessionBus();

    if (toggle)
        sessionBus.send(QDBusMessage::createMethodCall("org.nemomobile.lipstick", "/request", "org.nemomobile.lipstick", "showLockScreen"));
    else
        sessionBus.send(QDBusMessage::createMethodCall("org.nemomobile.lipstick", "/request", "org.nemomobile.lipstick", "hideLockScreen"));

    lockscreenVisible = toggle;
    emit screenIsLocked(toggle);
}

void ScreenLockBusinessLogic::toggleEventEater(bool toggle)
{
    if (toggle) {
        if (eventEaterWindow == NULL) {
            // Create the event eater window if it doesn't exist yet
            eventEaterWindow = new EventEater;
            eventEaterWindow->installEventFilter(new CloseEventEater(this));
            connect(eventEaterWindow, SIGNAL(inputEventReceived()), this, SLOT(hideEventEater()));
        }

        eventEaterWindow->show();
    } else {
        if (eventEaterWindow != NULL) {
            eventEaterWindow->hide();
        }
    }
}

bool ScreenLockBusinessLogic::isScreenLocked() const
{
    return lockscreenVisible;
}

#ifdef HAVE_QMSYSTEM
void ScreenLockBusinessLogic::systemStateChanged(MeeGo::QmSystemState::StateIndication what)
{
    switch (what) {
        case MeeGo::QmSystemState::Shutdown:
            // The lock screen should be disabled during shutdown
            hideScreenLockAndEventEater();
            shuttingDown = true;
            break;
        default:
            break;
    }
}
#endif
