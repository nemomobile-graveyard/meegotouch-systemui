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
#include <MApplicationIfProxy>
#include <MMessageBox>
#include "statusindicatormenu.h"
#include "statusindicatormenudropdownview.h"

const QString StatusIndicatorMenu::CONTROL_PANEL_SERVICE_NAME = "com.nokia.DuiControlPanel";

StatusIndicatorMenu::StatusIndicatorMenu(QGraphicsItem *parent) :
    MApplicationMenu()
{
#ifdef HAVE_QMSYSTEM
    m_State = NULL;
#endif
    Q_UNUSED(parent);
}

StatusIndicatorMenu::~StatusIndicatorMenu()
{
}

void StatusIndicatorMenu::setStatusIndicatorMenuInterface(MApplicationExtensionInterface *extension)
{
    MStatusIndicatorMenuExtensionInterface *menuExtension = static_cast<MStatusIndicatorMenuExtensionInterface *>(extension);
    menuExtension->setStatusIndicatorMenuInterface(*this);
}

void StatusIndicatorMenu::launchControlPanelAndHide()
{
    MApplicationIfProxy mApplicationIfProxy(CONTROL_PANEL_SERVICE_NAME, this);

    if (mApplicationIfProxy.connection().isConnected()) {
        mApplicationIfProxy.launch();
    } else {
        qWarning() << "Could not launch" << CONTROL_PANEL_SERVICE_NAME << "- DBus not connected?";
    }

    hideStatusIndicatorMenu();
}

void StatusIndicatorMenu::launchPowerOffDialog()
{
    // % "Are you sure you want to power off the device?"
    MMessageBox box(qtTrId("qtn_stat_menu_poweroff_query"), M::YesButton | M::NoButton);
    int result = box.exec();
    if (result == QMessageBox::Yes) {
#ifdef HAVE_QMSYSTEM
        if (m_State == NULL) {
            m_State = new MeeGo::QmSystemState(this);
        }
        m_State->set(MeeGo::QmSystemState::ShuttingDown);
#else
        start("halt -p");
#endif
    }
}

void StatusIndicatorMenu::showStatusIndicatorMenu()
{
    emit showRequested();
}

void StatusIndicatorMenu::hideStatusIndicatorMenu()
{
    emit hideRequested();
}
