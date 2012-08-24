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

#include "profile.h"
#include "profileplugin.h"
#include <MButton>
#include <MButtonGroup>
#include <QGraphicsLinearLayout>
#include <QDBusInterface>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MDialog>

Profile::Profile(ProfilePlugin *profilePlugin, QGraphicsItem *parent) :
    MContentItem(MContentItem::IconAndTwoTextLabels, parent),
    plugin(profilePlugin)
{
    setTitle("Profile");
    setSubtitle("Silent");
    setObjectName("StatusIndicatorMenuExtensionContentItem");
    setImageID("icon-m-profile-silent");
    connect(this, SIGNAL(clicked()), this, SLOT(showProfileDialog()));
}

Profile::~Profile()
{
}

void Profile::showProfileDialog()
{

  QDBusInterface launcher ("com.nokia.DuiControlPanel", "/",
                                  "com.nokia.DuiControlPanelIf");
  launcher.call ("appletPage", "Profile");
  // Hide the status indicator menu
  if(MStatusIndicatorMenuInterface *menu = plugin->statusIndicatorMenuInterface()) {
        menu->hideStatusIndicatorMenu();
  }
}
