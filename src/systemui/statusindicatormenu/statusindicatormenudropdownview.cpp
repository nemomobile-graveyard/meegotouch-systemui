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
#include <QGraphicsSceneMouseEvent>
#include <MSceneManager>
#include <MSceneWindow>
#include <MApplicationExtensionArea>
#include <MEscapeButtonPanel>
#include <MButton>
#include <MOverlay>
#include <MPannableViewport>
#include <QGraphicsLinearLayout>
#include <MWidgetView>
#include "notificationarea.h"
#include <MViewCreator>
#include <mstatusbar.h>
#include <QGraphicsAnchorLayout>
#include "statusindicatormenu.h"
#include "statusindicatormenudropdownview.h"
#include "statusindicatormenuwindowstyle.h"


void EventEaterWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}

PannedWidgetController::PannedWidgetController(QGraphicsItem *parent) :
    MWidgetController(parent),
    bottommostWidget_(NULL)
{
}

const QGraphicsWidget *PannedWidgetController::bottommostWidget() const
{
    return bottommostWidget_;
}

void PannedWidgetController::setBottommostWidget(const QGraphicsWidget *widget)
{
    bottommostWidget_ = widget;
}

void PannedWidgetController::setGeometry(const QRectF &rect)
{
    MWidgetController::setGeometry(rect);
    emit positionOrSizeChanged();
}

void PannedWidgetController::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}

void PannedWidgetController::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (bottommostWidget_) {
        // Test that the y position of the event is bigger than the bottom edge of the bottommost widget.
        // The calculations are done in this widget's coordinate space.
        if (event->pos().y() > bottommostWidget_->y() + bottommostWidget_->geometry().height()) {
            emit pressedOutSideContents();
        }
    }
}

StatusIndicatorMenuDropDownView::StatusIndicatorMenuDropDownView(StatusIndicatorMenu *controller) :
    MSceneWindowView(controller),
    controller(controller),
    statusBar(new MStatusBar),
    pannableViewport(NULL),
    closeButtonOverlay(NULL),
    backgroundWidget(new MWidgetController)
{
    // Show status bar
    controller->sceneManager()->appearSceneWindowNow(statusBar);

    // Create close button overlay
    closeButtonOverlay = createCloseButtonOverlay();

    // Create the pannable area background widget
    backgroundWidget->setView(new MWidgetView(backgroundWidget));
    backgroundWidget->setObjectName("StatusIndicatorMenuWindowBackground");

    connect(controller, SIGNAL(hideRequested()), this, SLOT(resetViewport()));
}


StatusIndicatorMenuDropDownView::~StatusIndicatorMenuDropDownView()
{
    delete statusBar;
    delete closeButtonOverlay;
}

QGraphicsWidget* StatusIndicatorMenuDropDownView::createTopRow()
{
    // Create an extension area for the top row plugins
    MApplicationExtensionArea *extensionArea = new MApplicationExtensionArea("com.meego.core.MStatusIndicatorMenuExtensionInterface/1.0");
    connect(extensionArea, SIGNAL(extensionInstantiated(MApplicationExtensionInterface*)), controller, SLOT(setStatusIndicatorMenuInterface(MApplicationExtensionInterface*)));
    extensionArea->setObjectName("StatusIndicatorMenuTopRowExtensionArea");
    extensionArea->setInProcessFilter(QRegExp("/statusindicatormenu-(alarms|internetconnection|presence|profile).desktop$"));
    extensionArea->setOutOfProcessFilter(QRegExp("$^"));
    extensionArea->setOrder((QStringList() << "statusindicatormenu-alarms.desktop" << "statusindicatormenu-internetconnection.desktop" << "statusindicatormenu-presence.desktop" << "statusindicatormenu-profile.desktop"));
    extensionArea->init();

    // Create a button for accessing the full settings
    //% "Settings"
    MButton *settingsButton = new MButton(qtTrId("qtn_stat_menu_settings"));
    settingsButton->setObjectName("StatusIndicatorMenuTopRowExtensionButton");
    settingsButton->setViewType(MButton::iconType);
    settingsButton->setIconID("icon-m-status-menu-settings");
    connect(settingsButton, SIGNAL(clicked()), controller, SLOT(launchControlPanelAndHide()));

    // Put the extension area and the settings button to a horizontal layout
    QGraphicsLinearLayout *topRowLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    topRowLayout->setContentsMargins(0, 0, 0, 0);
    topRowLayout->setSpacing(0);
    topRowLayout->addStretch();
    topRowLayout->addItem(extensionArea);
    topRowLayout->addItem(settingsButton);
    topRowLayout->addStretch();

    // Create a container widget for extension area and settings button layout
    MWidgetController *topRowWidget = new MWidgetController;
    topRowWidget->setView(new MWidgetView(topRowWidget));
    topRowWidget->setObjectName("StatusIndicatorMenuExtensionAreaWidget");
    topRowWidget->setLayout(topRowLayout);

    return topRowWidget;
}

MApplicationExtensionArea* StatusIndicatorMenuDropDownView::createVerticalExtensionArea()
{
    // Create an extension area for the call ui and transfer ui plugins
    MApplicationExtensionArea *extensionArea = new MApplicationExtensionArea("com.meego.core.MStatusIndicatorMenuExtensionInterface/1.0");
    extensionArea->setObjectName("StatusIndicatorMenuVerticalExtensionArea");
    extensionArea->setInProcessFilter(QRegExp("/statusindicatormenu-(call|transfer).desktop$"));
    extensionArea->setOutOfProcessFilter(QRegExp("$^"));
    extensionArea->setOrder((QStringList() << "statusindicatormenu-call.desktop" << "statusindicatormenu-transfer.desktop"));
    connect(extensionArea, SIGNAL(extensionInstantiated(MApplicationExtensionInterface*)), controller, SLOT(setStatusIndicatorMenuInterface(MApplicationExtensionInterface*)));
    extensionArea->init();
    return extensionArea;
}

MPannableViewport* StatusIndicatorMenuDropDownView::createPannableArea()
{
    // Create pannable area contents
    MApplicationExtensionArea *extensionArea = createVerticalExtensionArea();

    QGraphicsLinearLayout *contentLayout = new QGraphicsLinearLayout(Qt::Vertical);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->addItem(extensionArea);

    if(style()->notificationArea()) {
        NotificationArea *notificationArea = new NotificationArea;
        connect(notificationArea, SIGNAL(bannerClicked()), controller, SIGNAL(hideRequested()));
        contentLayout->addItem(notificationArea);
    }

    MWidgetController *contentWidget = new MWidgetController;
    contentWidget->setView(new MWidgetView(contentWidget));
    contentWidget->setObjectName("StatusIndicatorMenuContentWidget");
    contentWidget->setLayout(contentLayout);

    QGraphicsLinearLayout *pannableLayout = new QGraphicsLinearLayout(Qt::Vertical);
    pannableLayout->setContentsMargins(0, 0, 0, 0);
    pannableLayout->setSpacing(0);
    pannableLayout->addItem(contentWidget);
    QGraphicsWidget *closeButtonRow = createCloseButtonRow();
    pannableLayout->addItem(closeButtonRow);
    pannableLayout->addStretch();

    // Create a container widget for the pannable area
    PannedWidgetController *pannedWidget = new PannedWidgetController;
    pannedWidget->setView(new MWidgetView(pannedWidget));
    pannedWidget->setLayout(pannableLayout);
    pannedWidget->setBottommostWidget(closeButtonRow);
    connect(pannedWidget, SIGNAL(positionOrSizeChanged()), this, SLOT(setPannabilityAndLayout()));
    connect(pannedWidget, SIGNAL(pressedOutSideContents()), controller, SIGNAL(hideRequested()));

    // Setup the pannable viewport
    MPannableViewport *pannableViewport = new MPannableViewport;
    pannableViewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pannableViewport->setWidget(pannedWidget);
    return pannableViewport;
}

QGraphicsWidget* StatusIndicatorMenuDropDownView::createCloseButtonRow()
{
    // Create a close button for the pannable area
    MButton *closeButton = new MButton;
    closeButton->setViewType("icon");
    closeButton->setObjectName("StatusIndicatorMenuCloseButton");
    closeButton->setIconID("icon-m-framework-close");
    connect(closeButton, SIGNAL(clicked()), controller, SIGNAL(hideRequested()));

    // Add two overlay widgets that will not allow mouse events to pass through them next to the close button
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addItem(new EventEaterWidget);
    layout->addItem(closeButton);
    layout->addItem(new EventEaterWidget);

    // Create the area itself
    MWidgetController *closeButtonArea = new MWidgetController;
    closeButtonArea->setView(new MWidgetView(closeButtonArea));
    closeButtonArea->setObjectName("CloseButtonArea");
    closeButtonArea->setLayout(layout);

    return closeButtonArea;
}

MOverlay *StatusIndicatorMenuDropDownView::createCloseButtonOverlay()
{
    // Create a close button
    MButton *closeButton = new MButton;
    closeButton->setViewType("icon");
    closeButton->setObjectName("StatusIndicatorMenuCloseButton");
    closeButton->setIconID("icon-m-framework-close");
    connect(closeButton, SIGNAL(clicked()), controller, SIGNAL(hideRequested()));

    // Add two overlay widgets that will not allow mouse events to pass through them next to the close button
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addItem(new EventEaterWidget);
    layout->addItem(closeButton);
    layout->addItem(new EventEaterWidget);

    // Create the overlay itself
    MOverlay *closeButtonOverlay = new MOverlay;
    closeButtonOverlay->setLayout(layout);
    closeButtonOverlay->setObjectName("CloseButtonOverlay");
    controller->sceneManager()->appearSceneWindowNow(closeButtonOverlay);
    controller->sceneManager()->disappearSceneWindowNow(closeButtonOverlay);

    return closeButtonOverlay;
}

void StatusIndicatorMenuDropDownView::setPannabilityAndLayout()
{
    QGraphicsWidget *pannableWidget = pannableViewport->widget();

    // Enable pannability if there is too much content to fit on the screen
    bool viewportShouldBePannable = pannableWidget->effectiveSizeHint(Qt::PreferredSize).height() > pannableViewport->geometry().height();
    pannableViewport->setEnabled(viewportShouldBePannable);

    // Appear or disappear the close button overlay based on close area position
    const QGraphicsWidget *closeButtonRow = static_cast<PannedWidgetController *>(pannableViewport->widget())->bottommostWidget();
    qreal screenHeight = controller->sceneManager()->visibleSceneSize().height();
    qreal closeButtonRowBottomYPos = closeButtonRow->mapToItem(controller, QPointF(0, closeButtonRow->geometry().height())).y();

    if (closeButtonRowBottomYPos <= screenHeight) {
        controller->sceneManager()->disappearSceneWindowNow(closeButtonOverlay);
    } else {
        controller->sceneManager()->appearSceneWindowNow(closeButtonOverlay);
    }

    // Make the pannable area background window extend from the top of the pannable viewport halfway to the bottom of the close button row
    qreal viewPortYPos = pannableViewport->mapToItem(controller, QPointF()).y();
    qreal backgroundHeight = (closeButtonRowBottomYPos - viewPortYPos) / 2;
    if (backgroundHeight < 0) {
        backgroundHeight = 0;
    }
    backgroundWidget->setMinimumHeight(backgroundHeight);
    backgroundWidget->setMaximumHeight(backgroundHeight);
}


void StatusIndicatorMenuDropDownView::resetViewport()
{
    pannableViewport->setPosition(QPointF(0,0));
}


void StatusIndicatorMenuDropDownView::applyStyle()
{
    MWidgetView::applyStyle();

    if (pannableViewport == NULL) {
        QGraphicsAnchorLayout *backgroundLayout = new QGraphicsAnchorLayout;
        backgroundLayout->setContentsMargins(0, 0, 0, 0);
        backgroundLayout->setSpacing(0);
        backgroundLayout->addCornerAnchors(backgroundWidget, Qt::TopLeftCorner, backgroundLayout, Qt::TopLeftCorner);
        backgroundLayout->setMaximumHeight(0);

        // Put all the stuff into the scene window layout
        pannableViewport = createPannableArea();
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addItem(createTopRow());
        layout->addItem(backgroundLayout);
        layout->addItem(pannableViewport);
        controller->setLayout(layout);
    }
}

M_REGISTER_VIEW_NEW(StatusIndicatorMenuDropDownView, StatusIndicatorMenu)