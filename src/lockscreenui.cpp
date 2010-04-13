/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */
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
#include "lockscreenui.h"
#include "unlocksliderwidget/unlockslider.h"
#include "sysuid.h"

#include <QDebug>
#include <QGraphicsLinearLayout>

#include <DuiLabel>
#include <DuiImageWidget>
#include <DuiButton>
#include <DuiLayout>
#include <DuiLinearLayoutPolicy>
#include <DuiSceneManager>
#include <DuiApplicationWindow>
#include <DuiTheme>
#include <DuiLocale>

#include <QTime>
#include <QDateTime>

#include "duiwidgetcreator.h"
DUI_REGISTER_WIDGET_NO_CREATE(LockScreenUI)

#define DEBUG
#include "debug.h"

LockScreenUI::LockScreenUI () :
        m_TimeLabel (0),
        m_DateLabel (0)
{
    SYS_DEBUG ("");
    setPannableAreaInteractive (false);

    // let's hide home button
    setComponentsDisplayMode (DuiApplicationPage::AllComponents,
                              DuiApplicationPageModel::Hide);
}

LockScreenUI::~LockScreenUI ()
{
    SYS_DEBUG ("");
}

/*!
 * FIXME: This is a public method and there is no protection against multiple
 * calling.
 * FIXME: It is not clear why we need createWidgets() and createContent(), one
 * should be enough.
 */
void
LockScreenUI::createContent ()
{
    SYS_DEBUG ("");

    DuiApplicationPage::createContent();

    /*
     * I had to modify this function because it caused a leak, and that led us
     * to failed unit test. Now the screen looks a little bit different, but
     * that doesn't matter, because we have to change the UI according to the
     * new UI spec anyway. I left the old code here, so we can see what was the
     * original idea.
     */
    //DuiLayout* layout = new DuiLayout;
    DuiLayout* widgets = createWidgets();

#if 0
    QSize size = Sysuid::sysuid()->applicationWindow().sceneManager()->visibleSceneSize(Dui::Landscape);

    DuiGridLayoutPolicy* l_policy = new DuiGridLayoutPolicy(layout);
    l_policy->setSpacing(10);
    l_policy->setRowFixedHeight(1, size.height());
    l_policy->setColumnFixedWidth(0, size.width());
    l_policy->addItem(widgets, 1, 0, Qt::AlignCenter);

    size = Sysuid::sysuid()->applicationWindow().sceneManager()->visibleSceneSize(Dui::Portrait);

    DuiGridLayoutPolicy* p_policy = new DuiGridLayoutPolicy(layout);
    p_policy->setSpacing(10);
    p_policy->setRowFixedHeight(1, size.height());
    p_policy->setColumnFixedWidth(0, size.width());
    p_policy->addItem(widgets, 1, 0, Qt::AlignCenter);

    layout->setLandscapePolicy(l_policy);
    layout->setPortraitPolicy(p_policy);

#endif
    //centralWidget()->setLayout(layout);
    centralWidget()->setLayout(widgets);

    connect(slider, SIGNAL(unlocked()), this, SLOT(sliderUnlocked()));
}

void 
LockScreenUI::sliderUnlocked ()
{
    SYS_DEBUG ("");

    disappear ();
    slider->reset ();
    emit unlocked ();
}

DuiLayout *
LockScreenUI::createWidgets ()
{
    SYS_DEBUG ("");
    QGraphicsLinearLayout *datetimeBox;
    QGraphicsLinearLayout *lockliftBox;
    DuiLinearLayoutPolicy   *policy;
    DuiLayout             *layout;
    
    /*
     * The main layout and its policy
     */
    layout = new DuiLayout ();
    policy = new DuiLinearLayoutPolicy (layout, Qt::Vertical);

    /*
     * The label that shows the hour and minute
     */
    m_TimeLabel = new DuiLabel;
    m_TimeLabel->setObjectName ("lockscreenm_TimeLabel");

    /*
     * The label that shows the date
     */
    m_DateLabel = new DuiLabel;
    m_DateLabel->setObjectName ("lockscreenm_DateLabel");
    
    /*
     * The two images, one that we start the unlocking (source) and one that we
     * have to drop the source (target) to unlock the screen.
     *
     * icon-m-common-locked and icon-m-common-unlocked?
     */
    m_ImgSource = new DuiImageWidget ("icon-m-common-locked");
    m_ImgTarget = new DuiImageWidget ("icon-m-common-unlocked");

    datetimeBox = new QGraphicsLinearLayout (Qt::Vertical);
    datetimeBox->addItem (m_TimeLabel);
    datetimeBox->setAlignment (m_TimeLabel, Qt::AlignLeft);
    datetimeBox->addItem (m_DateLabel);
    datetimeBox->setAlignment (m_DateLabel, Qt::AlignLeft);
   
    lockliftBox = new QGraphicsLinearLayout (Qt::Horizontal);
    lockliftBox->addItem (datetimeBox);
    lockliftBox->setAlignment (datetimeBox, Qt::AlignLeft | Qt::AlignVCenter);
    lockliftBox->addItem (m_ImgSource);
    lockliftBox->setAlignment (m_ImgSource, Qt::AlignRight | Qt::AlignVCenter);

    /*
     * The slider: it is deprecated, but we use it until we have something else
     */
    slider = new UnlockSlider;
    slider->setSizePolicy (QSizePolicy (QSizePolicy::Expanding,
                                        QSizePolicy::Expanding));
    slider->setVisible (true);
    slider->setObjectName ("unlockslider");
    slider->setMinimumWidth (450);
    slider->setMaximumWidth (450);
    slider->setMaximumHeight (80);

    policy->addItem (lockliftBox);
    policy->addItem (slider, Qt::AlignCenter);
    policy->addItem (m_ImgTarget);

    updateDateTime();
    return layout;
}

void 
LockScreenUI::updateDateTime ()
{
    SYS_DEBUG ("");

    if (isContentCreated () == false)
        return;

    DuiLocale locale;

    QDateTime now (QDateTime::currentDateTime());

    m_TimeLabel->setText (locale.formatDateTime (
                now, DuiLocale::DateNone, DuiLocale::TimeShort));
    m_DateLabel->setText (locale.formatDateTime (
                now, DuiLocale::DateFull, DuiLocale::TimeNone));
    update();
}


