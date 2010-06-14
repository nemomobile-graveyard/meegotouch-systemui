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
#include "volumecontrolui.h"
#include "volumeoverlay.h"
#include "volumebarlogic.h"

VolumeControlUI::VolumeControlUI (QObject *parent) :
    QObject (parent),
    m_logic (new VolumeBarLogic),
    m_overlay (0)
#ifdef HAVE_QMSYSTEM
    , m_hwkeys (0)
#endif
{
#ifdef HAVE_QMSYSTEM
    m_hwkeys = new Maemo::QmKeys (this);
#endif

#ifdef HAVE_LIBRESOURCEQT
    m_hwkeyResource = new ResourcePolicy::ResourceSet ("event");
    m_hwkeyResource->setAlwaysReply ();

    ResourcePolicy::ScaleButtonResource *volumeKeys = new ResourcePolicy::ScaleButtonResource;

    m_hwkeyResource->addResourceObject (volumeKeys);

    connect (m_hwkeyResource, SIGNAL (resourcesGranted (QList<ResourcePolicy::ResourceType>)),
             this, SLOT (hwKeyResourceAcquired ()));
    connect (m_hwkeyResource, SIGNAL (lostResources ()),
             this, SLOT (hwKeyResourceLost ()));

    m_hwkeyResource->acquire ();
#endif
}

VolumeControlUI::~VolumeControlUI ()
{
#ifdef HAVE_LIBRESOURCEQT
    //Free the resources here
    m_hwkeyResource->deleteResource (ResourcePolicy::ScaleButtonType);
#endif

#ifdef HAVE_QMSYSTEM
    delete m_hwkeys;
    m_hwkeys = 0;
#endif

    delete m_logic;
    m_logic = 0;

    delete m_overlay;
    m_overlay = 0;
}

void
VolumeControlUI::overlayChanged (int val)
{
    // Set the volume value
    m_logic->setVolume ((quint32) val);
}

#ifdef HAVE_QMSYSTEM
void
VolumeControlUI::hwKeyEvent (QmKeys::Key key, QmKeys::State state)
{
    int change_val = 0;

    if (state == Maemo::QmKeys::KeyUp)
        return;

    switch (key)
    {
        case Maemo::QmKeys::VolumeUp:
            change_val++;
            break;
        case Maemo::QmKeys::VolumeDown:
            change_val--;
            break;
        default:
            // no-op for other hw-keys...
            return;
            break;
    }

    int current_volume = (int) m_logic->getVolume ();
    int max_volume = (int) m_logic->getMaxVolume ();

    current_volume += change_val;

    if (current_volume >= max_volume)
        current_volume = max_volume - 1;
    if (current_volume < 0)
        current_volume = 0;

    // This sets the volume and update the slider ...
    overlayChanged (current_volume);

    if (m_overlay == 0)
    {
        m_overlay = new VolumeOverlay;

        connect (m_overlay, SIGNAL (VolumeChanged (int)),
                 this, SLOT (overlayChanged (int)));
    }
    // ... and show the overlay
    m_overlay->UpdateVolume (current_volume, max_volume);
}
#endif

void
VolumeControlUI::hwKeyResourceAcquired ()
{
#ifdef HAVE_QMSYSTEM
    // Disconnect from everything first
    m_hwkeys->disconnect ();

    // TODO: use the fully qualified signal when it becomes available
    connect(m_hwkeys, SIGNAL(keyEvent(QmKeys::Key, QmKeys::State)), this, SLOT(hwKeyEvent(QmKeys::Key, QmKeys::State)));
#endif
}

void
VolumeControlUI::hwKeyResourceLost ()
{
#ifdef HAVE_QMSYSTEM
    m_hwkeys->disconnect ();
#endif
}
