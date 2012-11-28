/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of system ui.
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

#include "ut_sysuid.h"

#include <QtTest/QtTest>
#include <QDBusConnection>
#include <MApplication>
#include <MLocale>
#include "batterybusinesslogic_stub.h"
#include "statusarearenderer_stub.h"
#include "statusarearendereradaptor_stub.h"
#include "contextframeworkitem_stub.h"
#include "shutdownbusinesslogic_stub.h"
#include "shutdownbusinesslogicadaptor_stub.h"
#include "statusindicatormenubusinesslogic_stub.h"
#include "statusindicatormenuadaptor_stub.h"
#include "shutdownui_stub.h"
#include "usbui_stub.h"
#include "x11wrapper_modified_stub.h"
#include "closeeventeater_stub.h"
#include "diskspacenotifier_stub.h"
#include "testcontextitem.h"
#include "sysuid.h"

bool gQmLocksDeviceLock = false;
bool gQmLocksScreenLock = false;
bool gQmDisplayStateOff = false;

#ifdef HAVE_QMSYSTEM
namespace MeeGo
{

QmLocks::State QmLocks::getState(QmLocks::Lock what) const {
    if(what == QmLocks::Device)
        return gQmLocksDeviceLock ? QmLocks::Locked : QmLocks::Unlocked;
    else if(what == QmLocks::TouchAndKeyboard)
        return gQmLocksScreenLock ? QmLocks::Locked : QmLocks::Unlocked;
    else
        return QmLocks::Unknown;
}

bool QmLocks::setState(QmLocks::Lock, QmLocks::State) {
    return false;
}

bool QmLocks::setDeviceAutolockTime(int) {
    return false;
}

int QmLocks::getDeviceAutolockTime() {
    return 0;
}
}
#endif

// QDBusConnection stubs (used by Sysuid)
bool QDBusConnection::registerService(QString const &)
{
    return true;
}

bool QDBusConnection::registerObject(QString const &, QObject *, QFlags<QDBusConnection::RegisterOption>)
{
    return true;
}

QHash<const MLocale*, QSet<QString> > gInstalledTranslationCatalogs;
void MLocale::installTrCatalog(const QString &name)
{
    gInstalledTranslationCatalogs[this].insert(name);
}
const MLocale *gDefaultLocale;
void MLocale::setDefault(const MLocale &locale)
{
    gDefaultLocale = &locale;
}

// ContextFrameworkContext stub
TestContextItem *testContextItem;
ContextItem *ContextFrameworkContext::createContextItem(const QString&)
{
    testContextItem = new TestContextItem;
    return testContextItem;
}

void Ut_Sysuid::initTestCase()
{
    static char *args[] = {(char *) "./ut_sysuid"};
    static int argc = sizeof(args) / sizeof(char *);
    app = new MApplication(argc, args);
}

void Ut_Sysuid::cleanupTestCase()
{
    delete app;
}

void Ut_Sysuid::init()
{
    gInstalledTranslationCatalogs.clear();
    gDefaultLocale = NULL;
    sysuid = new Sysuid(NULL);
    gQmLocksDeviceLock = false;
    gQmLocksScreenLock = false;
}

void Ut_Sysuid::cleanup()
{
    delete sysuid;
    gStatusIndicatorMenuBusinessLogicStub->stubReset();
}

QTEST_APPLESS_MAIN(Ut_Sysuid)
