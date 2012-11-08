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

#include "statusareaview.h"
#include "statusarea.h"
#include "statusindicator.h"
#include "clock.h"
#include "contextframeworkcontext.h"
#include "inputmethodstatusindicatoradaptor.h"
#include "sysuid.h"
#include <QGraphicsLinearLayout>
#include <QGraphicsAnchorLayout>
#include <QDBusConnection>
#include <MDeviceProfile>
#include <MViewCreator>
#include <MStylableWidget>

StatusAreaView::StatusAreaView(StatusArea *controller) :
    MWidgetView(controller),
    landscapeWidget(new MStylableWidget),
    portraitWidget(new MStylableWidget),
    controller(controller),
    landscapePhoneNetworkIndicator(new PhoneNetworkStatusIndicator(contextFrameworkContext, controller)),
    portraitPhoneNetworkIndicator(new PhoneNetworkStatusIndicator(contextFrameworkContext, controller)),
    landscapeAlarmIndicator(new AlarmStatusIndicator(contextFrameworkContext, controller)),
    portraitAlarmIndicator(new AlarmStatusIndicator(contextFrameworkContext, controller)),
    landscapeBatteryIndicator(new BatteryStatusIndicator(contextFrameworkContext, controller)),
    portraitBatteryIndicator(new BatteryStatusIndicator(contextFrameworkContext, controller)),
    landscapePhoneSignalStrengthIndicator(new PhoneNetworkSignalStrengthStatusIndicator(contextFrameworkContext, controller)),
    portraitPhoneSignalStrengthIndicator(new PhoneNetworkSignalStrengthStatusIndicator(contextFrameworkContext, controller)),
    landscapePhoneNetworkTypeIndicator(new PhoneNetworkTypeStatusIndicator(contextFrameworkContext, controller)),
    portraitPhoneNetworkTypeIndicator(new PhoneNetworkTypeStatusIndicator(contextFrameworkContext, controller)),
    landscapeShortDistanceNetworkIndicator(new ShortDistanceNetworkStatusIndicator(contextFrameworkContext, controller)),
    portraitShortDistanceNetworkIndicator(new ShortDistanceNetworkStatusIndicator(contextFrameworkContext, controller)),
    landscapeGPSIndicator(new GPSStatusIndicator(contextFrameworkContext, controller)),
    portraitGPSIndicator(new GPSStatusIndicator(contextFrameworkContext, controller)),
    landscapePresenceIndicator(new PresenceStatusIndicator(contextFrameworkContext, controller)),
    portraitPresenceIndicator(new PresenceStatusIndicator(contextFrameworkContext, controller)),
    landscapeProfileIndicator(new ProfileStatusIndicator(contextFrameworkContext, controller)),
    portraitProfileIndicator(new ProfileStatusIndicator(contextFrameworkContext, controller)),
    landscapeInputMethodIndicator(new InputMethodStatusIndicator(controller)),
    landscapeCallIndicator(new CallStatusIndicator(contextFrameworkContext, controller)),
    portraitCallIndicator(new CallStatusIndicator(contextFrameworkContext, controller)),
    landscapeTetheringIndicator(new TetheringStatusIndicator(contextFrameworkContext, controller)),
    portraitTetheringIndicator(new TetheringStatusIndicator(contextFrameworkContext, controller)),
    landscapeCallForwardingIndicator(new CallForwardingStatusIndicator(contextFrameworkContext, controller)),
    portraitCallForwardingIndicator(new CallForwardingStatusIndicator(contextFrameworkContext, controller)),
    landscapeDLNAIndicator(new DLNAStatusIndicator(contextFrameworkContext, controller)),
    portraitDLNAIndicator(new DLNAStatusIndicator(contextFrameworkContext, controller)),
    landscapeTransferStatusIndicator(new TransferStatusIndicator(controller)),
    portraitTransferStatusIndicator(new TransferStatusIndicator(controller)),
    landscapeClock(new Clock(controller)),
    portraitClock(new Clock(controller)),
    callContextItem(contextFrameworkContext.createContextItem("Phone.Call"))
{
    // Set the style names of the landscape and portrait widgets when the call state changes
    setStyleNames();
    connect(callContextItem, SIGNAL(contentsChanged()), this, SLOT(setStyleNames()));
    callContextItem->subscribe();

    // Connect related phone network indicators
    connect(portraitPhoneSignalStrengthIndicator, SIGNAL(networkAvailabilityChanged(bool)), portraitPhoneNetworkTypeIndicator, SLOT(setNetworkAvailability(bool)));
    connect(landscapePhoneSignalStrengthIndicator, SIGNAL(networkAvailabilityChanged(bool)), landscapePhoneNetworkTypeIndicator, SLOT(setNetworkAvailability(bool)));

    // Set up the clock styles
    landscapeClock->setObjectName("StatusAreaLandscapeClock");
    portraitClock->setObjectName("StatusAreaPortraitClock");

    // Set up landscape and portrait widgets and anchor them on top of each other
    landscapeWidget->setLayout(createLandscapeLayout());
    landscapeWidget->setParent(controller);
    portraitWidget->setLayout(createPortraitLayout());
    portraitWidget->setParent(controller);
    QGraphicsAnchorLayout *compositeLayout = new QGraphicsAnchorLayout;
    compositeLayout->setContentsMargins(0, 0, 0, 0);
    compositeLayout->setSpacing(0);

    compositeLayout->addCornerAnchors(landscapeWidget, Qt::TopLeftCorner, compositeLayout, Qt::TopLeftCorner);
    compositeLayout->addCornerAnchors(landscapeWidget, Qt::TopRightCorner, compositeLayout, Qt::TopRightCorner);
    compositeLayout->addCornerAnchors(portraitWidget, Qt::TopLeftCorner, landscapeWidget, Qt::BottomLeftCorner);
    compositeLayout->addCornerAnchors(portraitWidget, Qt::BottomLeftCorner, compositeLayout, Qt::BottomLeftCorner);
    controller->setLayout(compositeLayout);

    // Connect to D-Bus and register the DBus source as an object
    new InputMethodStatusIndicatorAdaptor(landscapeInputMethodIndicator);
    QDBusConnection::sessionBus().registerService("com.meego.core.MInputMethodStatusIndicator");
    QDBusConnection::sessionBus().registerObject("/inputmethodstatusindicator", landscapeInputMethodIndicator);

    // Set different object names to PhoneNetworkStatusIndicator in landscape and portrait
    landscapePhoneNetworkIndicator->setObjectName(QString(landscapePhoneNetworkIndicator->metaObject()->className()) + "Landscape");
    portraitPhoneNetworkIndicator->setObjectName(QString(portraitPhoneNetworkIndicator->metaObject()->className()) + "Portrait");

    // Set up the class for functional testing
    setupTestability();
}

void StatusAreaView::setupTestability()
{
    portraitPhoneNetworkIndicator->setParent(portraitWidget);
    landscapeAlarmIndicator->setParent(landscapeWidget);
    portraitAlarmIndicator->setParent(portraitWidget);
    landscapeBatteryIndicator->setParent(landscapeWidget);
    portraitBatteryIndicator->setParent(portraitWidget);
    landscapePhoneSignalStrengthIndicator->setParent(landscapeWidget);
    portraitPhoneSignalStrengthIndicator->setParent(portraitWidget);
    landscapePhoneNetworkTypeIndicator->setParent(landscapeWidget);
    portraitPhoneNetworkTypeIndicator->setParent(portraitWidget);
    landscapeShortDistanceNetworkIndicator->setParent(landscapeWidget);
    portraitShortDistanceNetworkIndicator->setParent(portraitWidget);
    landscapeGPSIndicator->setParent(landscapeWidget);
    portraitGPSIndicator->setParent(portraitWidget);
    landscapePresenceIndicator->setParent(landscapeWidget);
    portraitPresenceIndicator->setParent(portraitWidget);
    landscapeProfileIndicator->setParent(landscapeWidget);
    portraitProfileIndicator->setParent(portraitWidget);
    landscapeInputMethodIndicator->setParent(landscapeWidget);
    landscapeCallIndicator->setParent(landscapeWidget);
    portraitCallIndicator->setParent(portraitWidget);
    landscapeTetheringIndicator->setParent(landscapeWidget);
    portraitTetheringIndicator->setParent(portraitWidget);
    landscapeClock->setParent(landscapeWidget);
    portraitClock->setParent(portraitWidget);
    landscapeCallForwardingIndicator->setParent(landscapeWidget);
    portraitCallForwardingIndicator->setParent(portraitWidget);
    landscapeDLNAIndicator->setParent(landscapeWidget);
    portraitDLNAIndicator->setParent(portraitWidget);
    landscapeTransferStatusIndicator->setParent(landscapeWidget);
    portraitTransferStatusIndicator->setParent(portraitWidget);
}

StatusAreaView::~StatusAreaView()
{
    delete callContextItem;
}

void StatusAreaView::setGeometry(const QRectF &rect)
{
    int screenWidth = MDeviceProfile::instance()->resolution().width();
    int screenHeight = MDeviceProfile::instance()->resolution().height();
    qreal areaHeight = rect.height() / 2;

    if(MDeviceProfile::instance()->orientationFromAngle(M::Angle0)==M::Portrait) {
        landscapeWidget->setMinimumSize(screenHeight, areaHeight);
        landscapeWidget->setMaximumSize(screenHeight, areaHeight);
        portraitWidget->setMinimumSize(screenWidth, areaHeight);
        portraitWidget->setMaximumSize(screenWidth, areaHeight);
    } else {
        landscapeWidget->setMinimumSize(screenWidth, areaHeight);
        landscapeWidget->setMaximumSize(screenWidth, areaHeight);
        portraitWidget->setMinimumSize(screenHeight, areaHeight);
        portraitWidget->setMaximumSize(screenHeight, areaHeight);
    }
}

QGraphicsLinearLayout* StatusAreaView::createLandscapeLayout()
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Put indicators into the layout
    layout->addItem(landscapeBatteryIndicator);
    layout->addItem(landscapePhoneSignalStrengthIndicator);
    layout->addItem(landscapePhoneNetworkIndicator);
    layout->addItem(landscapePhoneNetworkTypeIndicator);
    layout->addStretch();
    layout->addItem(landscapeTransferStatusIndicator);
    layout->addItem(landscapeCallIndicator);
    layout->addItem(landscapeTetheringIndicator);
    layout->addItem(landscapeCallForwardingIndicator);
    layout->addItem(landscapeDLNAIndicator);
    layout->addItem(landscapeShortDistanceNetworkIndicator);
    layout->addItem(landscapeGPSIndicator);
    layout->addItem(landscapePresenceIndicator);
    layout->addItem(landscapeProfileIndicator);
    layout->addItem(landscapeInputMethodIndicator);
    layout->addItem(landscapeAlarmIndicator);
    layout->addItem(landscapeClock);

    return layout;
}

QGraphicsLinearLayout* StatusAreaView::createPortraitLayout()
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Put indicators into the layout
    layout->addItem(portraitBatteryIndicator);
    layout->addItem(portraitPhoneSignalStrengthIndicator);
    layout->addItem(portraitPhoneNetworkIndicator);
    layout->addItem(portraitPhoneNetworkTypeIndicator);
    layout->addStretch();
    layout->addItem(portraitTransferStatusIndicator);
    layout->addItem(portraitCallIndicator);
    layout->addItem(portraitTetheringIndicator);
    layout->addItem(portraitCallForwardingIndicator);
    layout->addItem(portraitDLNAIndicator);
    layout->addItem(portraitShortDistanceNetworkIndicator);
    layout->addItem(portraitGPSIndicator);
    layout->addItem(portraitPresenceIndicator);
    layout->addItem(portraitProfileIndicator);
    layout->addItem(portraitAlarmIndicator);
    layout->addItem(portraitClock);

    return layout;
}

void StatusAreaView::setStyleNames()
{
    // Set the style based on the call state
    if (callContextItem->value().toString() == "active") {
        landscapeWidget->setStyleName("StatusBarLandscapeWidgetCall");
        portraitWidget->setStyleName("StatusBarPortraitWidgetCall");
    } else {
        landscapeWidget->setStyleName("StatusBarLandscapeWidget");
        portraitWidget->setStyleName("StatusBarPortraitWidget");
    }
}

M_REGISTER_VIEW_NEW(StatusAreaView, StatusArea)
