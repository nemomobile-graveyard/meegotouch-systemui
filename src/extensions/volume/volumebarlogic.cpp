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
#include <QVariant>
#include <QString>

#include "volumebarwindow.h"
#include "closeeventeater.h"
#include "volumebarlogic.h"


VolumeBarLogic::VolumeBarLogic(QObject *parent) :
    QObject(parent),
    volumeBarWindow(NULL),
    currentvolume (0),
    currentmax (0),
    hwkeys(new keySniffer()),
    mHideTimer(new QTimer())  
{
    mHideTimer->setSingleShot(TRUE);
    openConnection();    

#if (HAVE_LIBRESOURCEQT)
    hwkeyResource = new ResourcePolicy::ResourceSet("event");
    hwkeyResource->setAlwaysReply();

    ResourcePolicy::ScaleButtonResource *volumeKeys = new ResourcePolicy::ScaleButtonResource;
    hwkeyResource->addResourceObject(volumeKeys);
    connect(hwkeyResource, SIGNAL(resourcesGranted(QList<ResourcePolicy::ResourceType>)), this, SLOT(hwKeyResourceAcquired()));
    connect(hwkeyResource, SIGNAL(lostResources()), this, SLOT(hwKeyResourceLost()));    

    hwkeyResource->acquire();
#endif
}

VolumeBarLogic::~VolumeBarLogic ()
{
#if (HAVE_LIBRESOURCEQT)
    hwkeyResource->deleteResource(ResourcePolicy::ScaleButtonType);
#endif

    delete volumeBarWindow;
}

void VolumeBarLogic::openConnection ()
{
  if(!mPAThread){
    mPAThread = new pulseaudio_thread();
    connect(mPAThread,SIGNAL(volumeChanged(quint32,quint32)),this,SLOT(stepsUpdated(quint32,quint32)));
    connect(this,SIGNAL(changeVolume(quint32)),mPAThread,SLOT(changeVolume(quint32)));
    mPAThread->start();
    hwkeys->start();
  }
}

void VolumeBarLogic::stepsUpdated (quint32 value, quint32 maxvalue)
{
    currentvolume = value;
    currentmax = maxvalue;
}

void VolumeBarLogic::setVolume (quint32 value)
{
  currentvolume = value;

    emit changeVolume(value);
}

quint32 VolumeBarLogic::volume ()
{
  return currentvolume;
}

quint32 VolumeBarLogic::maxVolume ()
{
  return currentmax;
}

void VolumeBarLogic::hwKeyEvent(uint key, int state)
{
  static int counter = 0; 
  counter++;
   
   if (state == KeyRelease) {
        // Do nothing on key releases
        return;
    }

    quint32 volumeChange = 0;
    switch (key) {
    case KEY_VOLUMEUP:
    case KEY_F7:
        volumeChange = 1;
        break;
    case KEY_VOLUMEDOWN:
    case KEY_F8:
        volumeChange = -1;
        break;
    default:
        // no-op for other hw keys
        return;
    }

    int volume = currentvolume + volumeChange;

    // Keep the volume within limits
    if (volume >= (int)currentmax) {
        volume = currentmax - 1;
    } else if (volume < 0) {
        volume = 0;
    }

    // Take the new volume into use
    emit changeVolume((quint32)volume);
    setVolume(volume);

    // Create the volume bar window if it does not exist yet
    if (volumeBarWindow == NULL) {
        volumeBarWindow = new VolumeBarWindow(this);
        volumeBarWindow->installEventFilter(new CloseEventEater(this));
        connect(mHideTimer, SIGNAL(timeout()), volumeBarWindow, SLOT(hide()));
    }

    // Show the volume bar if the window is not visible
    if (!volumeBarWindow->isVisible()) {
        volumeBarWindow->show();
    }
    volumeBarWindow->raise();
    mHideTimer->start(VOLUMECLICKTIMEOUT);

    // Update the volume bar geometry
    volumeBarWindow->updateVolume();
}

#if (HAVE_LIBRESOURCEQT)
void VolumeBarLogic::hwKeyResourceAcquired()
{
   // Disconnect from everything first
    hwkeys->disconnect ();

    connect(hwkeys, SIGNAL (keyPress(uint, int)), this, SLOT (hwKeyEvent (uint, int)));
}

void VolumeBarLogic::hwKeyResourceLost()
{
  hwkeys->disconnect();
}
#endif
