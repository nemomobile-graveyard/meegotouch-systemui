/****************************************************************************
**
** Copyright(C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation(directui@nokia.com)
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
#ifndef UT_LOCKSCREEN_H
#define UT_LOCKSCREEN_H

#include <QtTest/QtTest>
#include <QObject>
#include <QPointer>

#ifdef HAVE_QMSYSTEM
#include <qmdisplaystate.h>
#endif

class MApplication;
class MWindow;
class LockScreen;

class Ut_LockScreen : public QObject
{
    Q_OBJECT

signals:
#ifdef HAVE_QMSYSTEM
    void displayStateChanged(MeeGo::QmDisplayState::DisplayState displayState);
#endif
    void unlocked();

private slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

#ifdef HAVE_QMSYSTEM
    void testSliderUnlocked_data();
#endif
    void testSliderUnlocked();
    void testConstruction();
    void testWhenDisplayExitsLockScreenIsUnlocked();

private:
    MApplication *app;
    MWindow *parent;
    LockScreen  *lockScreen;
};

#endif
