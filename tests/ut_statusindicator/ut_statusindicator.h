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
#ifndef _UT_STATUSINDICATOR_
#define _UT_STATUSINDICATOR_

#include <QObject>

class MApplication;
class TestContext;
class StatusIndicator;

class Ut_StatusIndicator : public QObject
{
    Q_OBJECT

private:
    MApplication *app;
    TestContext *testContext;
    StatusIndicator *m_subject;

private slots:
    // Executed once before every test case
    void init();

    // Executed once after every test case
    void cleanup();

    // Executed once before first test case
    void initTestCase();

    // Executed once after last test case
    void cleanupTestCase();

    void testModelUpdates();
    void testContextItemSubscribe();
    void testContextItemDeletion();
    void testPhoneNetworkSignalStrength();
    void testPhoneNetworkSignalStrengthStyleName_data();
    void testPhoneNetworkSignalStrengthStyleName();
    void testWhenPhoneNetworkBecomesAvailableThenSignalIsEmitted();
    void testWhenPhoneNetworkBecomesUnavailableThenSignalIsEmitted();
    void testPhoneNetworkTypeStyleNameWhenSystemOnlineName_data();
    void testPhoneNetworkTypeStyleNameWhenSystemOnlineName();
    void testPhoneNetworkAvailabilityChanged_data();
    void testPhoneNetworkAvailabilityChanged();
    void testBattery();
    void testAlarm();
    void testShortDistanceNetwork_data();
    void testShortDistanceNetwork();
    void testPresence_data();
    void testPresence();
    void testAnimation();
    void testPhoneNetwork_data();
    void testPhoneNetwork();
    void testInputMethod();
    void testCall_data();
    void testCall();
    void testTethering();
    void testProfile_data();
    void testProfile();
    void testGPS();
    void testTransferStatusIndicatorDBusConnectionCreationInConstructor();
    void testTransferStatusStateChange();
    void testCallForwarding();
    void testDLNA();
};

#endif //_UT_STATUSINDICATOR_
