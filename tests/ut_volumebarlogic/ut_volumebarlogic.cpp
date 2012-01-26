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
#include <QtTest/QtTest>
#include <MApplication>
#include <MApplicationService>
#include <MWindow>
#include <MGConfItem>
#include <mcomponentdata.h>
#include <mfeedbackplayer.h>
#include "ut_volumebarlogic.h"
#include "volumebarlogic.h"
#include "volumebarwindow_stub.h"
#include "closeeventeater_stub.h"
#include "pulseaudio_thread.h"
#include "keysniffer.h"

#if (HAVE_LIBRESOURCEQT)
#include <policy/resource-set.h>
/*********************************************************************************
 * Stub for ResourcePolicy
 */

namespace ResourcePolicy
{
    ResourceSet::ResourceSet(const QString&, QObject*)
    {
    }

    bool ResourceSet::setAlwaysReply ()
    {
        return true;
    }

    ScaleButtonResource::ScaleButtonResource()
    {
    }

    void ResourceSet::addResourceObject (ResourcePolicy::Resource* rsc)
    {
        Q_UNUSED(rsc);
        return;
    }

    bool ResourceSet::acquire ()
    {
        return true;
    }

    void ResourceSet::deleteResource (ResourcePolicy::ResourceType)
    {
        return;
    }
}
#endif

MWindow::MWindow(QWidget *) : d_ptr(NULL)
{
}

MWindow::~MWindow()
{
}

void MWindow::setVisible(bool)
{
}

MGConfItem::MGConfItem(const QString &, QObject *parent) : QObject(parent), priv(NULL)
{
}

MGConfItem::~MGConfItem()
{
}

QVariant MGConfItem::value(const QVariant &def) const
{
    return def;
}

QVariant MGConfItem::value() const
{
    return QVariant();
}

QString MGConfItem::key() const
{
    return QString();
}

void Ut_VolumeBarLogic::initTestCase ()
{
    static int argc = 2;
    static char *argv[] = { (char *)"./ut_volumebarwindow", (char *)"-disable-m-input-context", 0 };
    app = new MApplication(argc, argv);
}

void Ut_VolumeBarLogic::cleanupTestCase ()
{
}

void Ut_VolumeBarLogic::init ()
{
    volumeBarLogic = new VolumeBarLogic;
    volumeBarLogic->stepsUpdated (30, 100);
    resetStubs();
}

void Ut_VolumeBarLogic::cleanup ()
{
    delete volumeBarLogic;
}

void Ut_VolumeBarLogic::testInitValues()
{
    resetStubs();
}


// Check if setting / getting works correctly
void Ut_VolumeBarLogic::testVolumeSetGet ()
{
    quint32 val = 5;

    volumeBarLogic->setVolume (val);

    QCOMPARE (volumeBarLogic->volume (), val);
}

void Ut_VolumeBarLogic::testVolumeChangeByPa ()
{
    quint32 currentstep = 10;
    quint32 stepcount   = 20;

    // A D-bus message calls this slot [inside the logic],
    // now we call this outside :
    volumeBarLogic->stepsUpdated (currentstep, stepcount);

    // Check the current ...
    QCOMPARE (volumeBarLogic->volume (), currentstep);
    // .. and the maximal values
    QCOMPARE (volumeBarLogic->maxVolume (), stepcount);
}

void Ut_VolumeBarLogic::testWhenPulseAudioSetsVolumeNewVolumeIsSet()
{
    // Do what PulseAudio do [of course Pa doing this indirectly...]
    volumeBarLogic->stepsUpdated (10, 100);
    QCOMPARE(volumeBarLogic->currentvolume, (uint)10);
    QCOMPARE(volumeBarLogic->currentmax, (uint)100);
}

void Ut_VolumeBarLogic::resetStubs()
{
    return;
}

#if (HAVE_LIBRESOURCEQT)
void Ut_VolumeBarLogic::testHwKeyEvent()
{
    // when current_volume >= max_volume
    volumeBarLogic->setVolume(120);
    volumeBarLogic->hwKeyEvent(115, 2);
    QCOMPARE(volumeBarLogic->volume(), 99u);

    // Turn up the volume
    volumeBarLogic->setVolume(20);
    volumeBarLogic->hwKeyEvent(115, 2);
    QCOMPARE(volumeBarLogic->volume(), 21u);

    // Turn down the volume
    volumeBarLogic->setVolume(20);
    volumeBarLogic->hwKeyEvent(114, 2);
    QCOMPARE(volumeBarLogic->volume(), 19u);

    volumeBarLogic->setVolume(20);
    volumeBarLogic->hwKeyEvent(115, 3);
    QCOMPARE(volumeBarLogic->volume(), 20u);

    // push some other hw key
    volumeBarLogic->setVolume(20);
    volumeBarLogic->hwKeyEvent(110, 2);
    QCOMPARE(volumeBarLogic->volume(), 20u);
}
#endif


QTEST_APPLESS_MAIN(Ut_VolumeBarLogic)
