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
#include "batterybusinesslogic.h"
#include "systemuigconf.h"

#include <MLocale>
#include <QTimer>
#include <QTime>

#include <MNotification>
#include <MGConfItem>

using namespace Maemo;

/* TODO List

   1) What are correct animation rates when charging with USB / Wall?
   2) If USB 100 mA is used, do we show animation at all? In Fremantle not.
   3) Connect sounds with notifications

*/

namespace
{
//% "Charging"
const char *ChargingText = QT_TRID_NOOP ("qtn_ener_charging");
//% "Charging not started. Replace charger."
const char *ChargingNotStartedText = QT_TRID_NOOP ("qtn_ener_repcharger");
//% "Charging complete"
const char *ChargingCompleteText = QT_TRID_NOOP ("qtn_ener_charcomp");
//% "Disconnect charger from power supply to save energy"
const char *DisconnectChargerText = QT_TRID_NOOP ("qtn_ener_remcha");
//% "Low battery"
const char *LowBatteryText = QT_TRID_NOOP ("qtn_ener_lowbatt");
//% "Entering power save mode"
const char *EnterPSMText = QT_TRID_NOOP ("qtn_ener_ent_psnote");
//% "Exiting power save mode"
const char *ExitPSMText = QT_TRID_NOOP ("qtn_ener_exit_psnote");
//% "Recharge battery"
const char *RechargeBatteryText = QT_TRID_NOOP ("qtn_ener_rebatt");

const int   LowBatteryActiveInterval = 30 * 60 * 1000; //30 mins
const int   LowBatteryInactiveInterval = 2 * 60 * 60 * 1000; //2 hours
const int   ChargingAnimationRateUSB = 800; // 800 ms
const int   ChargingAnimationRateWall = 400; // 400 ms
}

//#define DEBUG
#include "debug.h"

// This macro will hide the previous notification
#ifndef UNIT_TEST
#define NOTIFICATION(text) \
        if (m_notification != 0) \
        { \
            m_notification->remove (); \
            delete m_notification; \
            m_notification = 0; \
        } \
        m_notification = new MNotification (MNotification::DeviceEvent, "", (text)); \
        m_notification->publish ();
#else
// Do nothing on unit testing (no need to show the notification,
//                             and that could cause slow-down...)
#define NOTIFICATION(text) \
        SYS_DEBUG("UNIT_TEST Notification: \"%s\"",SYS_STR(text));
#endif

/******************************************************************************
 * Methods for the LowBatteryNotifier class.
 */
LowBatteryNotifier::LowBatteryNotifier (
        QObject *parent) :
        QObject (parent),
        m_Display (new QmDisplayState ()),
        m_Timer (new QTimer (this)),
        m_notification (0)
{
    SYS_DEBUG ("----------------------------------------------");

    m_Sleep = m_Display->get () == QmDisplayState::Off;
    m_ActiveInterval = LowBatteryActiveInterval;
    m_InactiveInterval = LowBatteryInactiveInterval;
    m_Time.start ();

    connect (m_Display,
            SIGNAL (displayStateChanged (Maemo::QmDisplayState::DisplayState)),
            this,
            SLOT (displayStateChanged (Maemo::QmDisplayState::DisplayState)));

    connect (m_Timer, SIGNAL (timeout ()),
            this, SLOT (showLowBatteryNotification ()));
}

LowBatteryNotifier::~LowBatteryNotifier ()
{
}


void
LowBatteryNotifier::showLowBatteryNotification ()
{
    SYS_DEBUG ("");

    NOTIFICATION (qtTrId (LowBatteryText));

#ifdef UNIT_TEST
    emit showNotification ();
#endif
    m_Time.start (); //restart time

    switch (m_Display->get ()) {
        case QmDisplayState::On:
        case QmDisplayState::Dimmed:
            m_Sleep = false;
            m_Timer->start (m_ActiveInterval);
            break;

        case QmDisplayState::Off:
            m_Sleep = true;
            m_Timer->start (m_InactiveInterval);
            break;

        default:
            // FIXME: what about the other states [Unknown]?
            SYS_WARNING ("Unknown display state");
            break;
    }
}

void
LowBatteryNotifier::displayStateChanged (
        Maemo::QmDisplayState::DisplayState state)
{
    SYS_DEBUG ("");

    switch (state) {
        case QmDisplayState::On:
            if (!m_Sleep)
                break;
            if (m_Time.elapsed () < m_ActiveInterval)
                m_Timer->setInterval (m_ActiveInterval - m_Time.elapsed ());
            else
                showLowBatteryNotification ();
            m_Sleep = false;
            break;

        case QmDisplayState::Dimmed:
            m_Sleep = false;
            break;

        case QmDisplayState::Off:
            m_Timer->setInterval (m_InactiveInterval - m_Time.elapsed ());
            m_Sleep = true;
            break;

        default:
            // FIXME: what about the other states [Unknown]?
            break;
    }
}

/******************************************************************************
 * Methods for the BatteryBusinessLogic class.
 */
BatteryBusinessLogic::BatteryBusinessLogic (
        SystemUIGConf *systemUIGConf,
        QObject       *parent) :
    QObject (parent),
    m_SystemUIGConf (systemUIGConf),
    m_Battery (new QmBattery),
    m_DeviceMode (new QmDeviceMode),
    m_Led (new QmLED),
    m_LowBatteryNotifier (0),
    m_notification (0)
{
    SYS_DEBUG ("----------------- start ----------------------");

    /*
     * Initializing the automatic power save mode possible values list. 
     */
    m_PSMThresholds = m_SystemUIGConf->value (
            SystemUIGConf::BatteryPSMThresholdsKey).toStringList();

    if (m_PSMThresholds.isEmpty()) {
        SYS_DEBUG ("m_PSMThresholds is empty: Setting the wired in values!!");

        m_PSMThresholds << "10" << "20" << "30" << "40" << "50";
        m_SystemUIGConf->setValue (
                SystemUIGConf::BatteryPSMThresholdsKey,
                QVariant(m_PSMThresholds));
    }

    /*
     * Initializing a list for the battery icons/bars.
     */
    m_BarValues <<
	    QString ("5") <<
	    QString ("10") <<
	    QString ("15") <<
	    QString ("25") <<
	    QString ("35") <<
	    QString ("50") <<
	    QString ("60") <<
	    QString ("75") <<
	    QString ("85") <<
	    QString ("100");

    /* check if gconfvalues need to initialized */
    initSystemUIGConfKeys ();

    /* connect to QmSystem signals */
    SYS_DEBUG ("Connecting QmSystem signals");
    connect (m_Battery,
             SIGNAL (batteryStateChanged (Maemo::QmBattery::BatteryState)),
             this,
             SLOT (batteryStateChanged (Maemo::QmBattery::BatteryState)));
    connect (m_Battery,
             SIGNAL (chargingStateChanged (Maemo::QmBattery::ChargingState)),
             this,
             SLOT (chargingStateChanged (Maemo::QmBattery::ChargingState)));
    connect (m_Battery,
             SIGNAL (batteryEnergyLevelChanged (int)),
             this,
             SLOT (batteryEnergyLevelChanged (int)));
    connect (m_Battery,
             SIGNAL (chargerEvent (Maemo::QmBattery::ChargerType)),
             this,
             SLOT (batteryChargerEvent (Maemo::QmBattery::ChargerType)));
    connect (m_DeviceMode,
             SIGNAL (devicePSMStateChanged (Maemo::QmDeviceMode::PSMState)),
             this,
             SLOT (devicePSMStateChanged (Maemo::QmDeviceMode::PSMState)));

    // Init battery values delayed...
    QTimer::singleShot (3000, this, SLOT (initBattery ()));

    SYS_DEBUG ("------------------ end -----------------------");
}


BatteryBusinessLogic::~BatteryBusinessLogic ()
{
    delete m_Battery;
    m_Battery = NULL;

    delete m_DeviceMode;
    m_DeviceMode = NULL;

    delete m_Led;
    m_Led = NULL;
}

void
BatteryBusinessLogic::initSystemUIGConfKeys ()
{
    // What?! This is not robust!
    if (m_SystemUIGConf->keyCount (SystemUIGConf::Battery) < 2) {
        /*
         * GConf keys have not yet been set.
         */
        m_SystemUIGConf->setValue (
                SystemUIGConf::BatteryPSMAutoKey,
                QVariant (true));
        m_SystemUIGConf->setValue (
                SystemUIGConf::BatteryPSMThresholdKey,
                QVariant (1));
    }
}


// This method should be called also when the device is returned from sleep
// mode
void
BatteryBusinessLogic::initBattery ()
{
    SYS_DEBUG ("");
    //init the charging status
    chargingStateChanged (m_Battery->getChargingState ());

    //init the battery level
    batteryStateChanged (m_Battery->getBatteryState ());
}

int
BatteryBusinessLogic::batteryBarValue (
        int percentage)
{
    SYS_DEBUG ("percentage = %d", percentage);

    if (percentage < 0)
        percentage = m_Battery->getBatteryEnergyLevel ();

    // in case of overflow (eg. in sbox) when we get value that greater than 100
    // percent we use 10 percent intentionaly
    if (percentage < 0)
        percentage = 0;
    else if (percentage > 100)
        percentage = 10;

    int index = 0;
    if (percentage >= 84)
        index = m_BarValues.indexOf ("100");
    else if (percentage < 84 && percentage >= 73)
        index = m_BarValues.indexOf ("85");
    else if (percentage < 73 && percentage >= 62)
        index = m_BarValues.indexOf ("75");
    else if (percentage < 62 && percentage >= 51)
        index = m_BarValues.indexOf ("60");
    else if (percentage < 51 && percentage >= 39)
        index = m_BarValues.indexOf ("50");
    else if (percentage < 39 && percentage >= 28)
        index = m_BarValues.indexOf ("35");
    else if (percentage < 28 && percentage >= 17)
        index = m_BarValues.indexOf ("25");
    else if (percentage < 17 && percentage >= 5)
        index = m_BarValues.indexOf ("15");
    else if (percentage < 5 && percentage > 1)
        index = m_BarValues.indexOf ("10");
    else // if (percentage == 0)
        index = m_BarValues.indexOf ("5");

    return index;
}

void
BatteryBusinessLogic::chargingStateChanged (
        QmBattery::ChargingState state)
{
    SYS_DEBUG ("");

    switch (state) {
        case QmBattery::StateCharging:
            SYS_DEBUG ("Charging");
            emit batteryCharging (animationRate (m_Battery->getChargerType ()));
            utiliseLED (true, QString ("PatternBatteryCharging"));
            NOTIFICATION (qtTrId (ChargingText));
            if (m_LowBatteryNotifier != 0) {
                delete m_LowBatteryNotifier;
                m_LowBatteryNotifier = 0;
            }
            break;

        case QmBattery::StateNotCharging:
            SYS_DEBUG ("Not charging");
            emit batteryNotCharging ();
            utiliseLED (false, QString ("PatternBatteryCharging"));
            break;

        case QmBattery::StateChargingFailed:
            SYS_DEBUG ("Charging not started");
            emit batteryNotCharging ();
            utiliseLED (false, QString ("PatternBatteryCharging"));
            NOTIFICATION (qtTrId (ChargingNotStartedText));
            break;

        default:
            SYS_WARNING ("Unhandled state: %d", state);
    }

    emit remainingTimeValuesChanged (remainingTimeValues ());
}

void
BatteryBusinessLogic::batteryStateChanged (
        QmBattery::BatteryState state)
{
    SYS_DEBUG ("");

    switch (state) {
    case QmBattery::StateFull:
        SYS_DEBUG ("QmBattery::StateFull");
        if (m_Battery->getChargingState () == QmBattery::StateCharging)
        {
            NOTIFICATION (qtTrId (ChargingCompleteText));
            utiliseLED (true, QString ("PatternBatteryFull"));
            emit batteryFullyCharged ();
        }
        break;

    case QmBattery::StateOK:
        SYS_DEBUG ("QmBattery::StateOK");
        /* no-operation here... */
        break;

    case QmBattery::StateLow:
        SYS_DEBUG ("QmBattery::StateLow");
        if (m_Battery->getChargingState () != QmBattery::StateCharging) {
            if (m_LowBatteryNotifier == 0) {
                m_LowBatteryNotifier = new LowBatteryNotifier ();
                #ifdef UNIT_TEST
                connect (m_LowBatteryNotifier, SIGNAL (showNotification ()),
                    this, SIGNAL (showNotification ()));
                #endif
            }
            m_LowBatteryNotifier->showLowBatteryNotification ();
        }
        break;

    case QmBattery::StateEmpty:
        SYS_DEBUG ("QmBattery::StateEmpty");
#ifdef __ARMEL__
        NOTIFICATION (qtTrId (RechargeBatteryText));
#endif
        break;

    case QmBattery::StateError:
        // TODO: Get information somewhere...
        SYS_WARNING ("battery state : error ? what about this state?");
        break;
    }
}

void
BatteryBusinessLogic::batteryEnergyLevelChanged (
        int percentage)
{
    SYS_DEBUG ("*** percentage = %d", percentage);

    emit batteryBarValueChanged (batteryBarValue (percentage));
    emit remainingTimeValuesChanged (remainingTimeValues ());
    checkPSMThreshold ();
}

void
BatteryBusinessLogic::batteryChargerEvent (
        Maemo::QmBattery::ChargerType type)
{
    SYS_DEBUG ("");

    switch (type) {
        case QmBattery::None: 
            SYS_DEBUG ("QmBattery::None");
            // No  charger connected
            if (m_Battery->getBatteryState () == QmBattery::StateFull) {
                NOTIFICATION (qtTrId (DisconnectChargerText)); //show reminder
	        }
            break;

        case QmBattery::Wall: 
            // Wall charger
            SYS_DEBUG ("QmBattery::Wall");
        case QmBattery::USB_500mA: 
            // USB with 500mA output
            SYS_DEBUG ("QmBattery::USB_500mA");
        case QmBattery::USB_100mA: 
            // USB with 100mA output
            SYS_DEBUG ("QmBattery::USB_100mA");
            emit batteryCharging (animationRate (type));
            break;

        default: 
            //QmBattery::Unknown
            SYS_WARNING ("Unknown charger state");
            break;
    }
}

void
BatteryBusinessLogic::devicePSMStateChanged (
        QmDeviceMode::PSMState PSMState)
{
    if (PSMState == QmDeviceMode::PSMStateOff) {
        NOTIFICATION (qtTrId (ExitPSMText));
        SYS_DEBUG ("Emitting DBus signal on PSM off");
        emit PSMValueChanged (false);
    } else if (PSMState == QmDeviceMode::PSMStateOn) {
        NOTIFICATION (qtTrId (EnterPSMText));
        SYS_DEBUG ("Emitting DBus signal on PSM on");
        emit PSMValueChanged (true);
    }

    emit remainingTimeValuesChanged (remainingTimeValues ());
}

void
BatteryBusinessLogic::checkPSMThreshold ()
{
    SYS_DEBUG ("");

    if (m_SystemUIGConf->value (SystemUIGConf::BatteryPSMAutoKey).toBool ()) {
        // we only handle this if the auto PSM is on
        if (batteryBarValue (m_Battery->getBatteryEnergyLevel ()) <=
                m_SystemUIGConf->value (SystemUIGConf::BatteryPSMThresholdKey).toInt ()) {
            if (m_DeviceMode->getPSMState () == QmDeviceMode::PSMStateOff)
                setPSMState (true);
        } else {
            if (m_DeviceMode->getPSMState () == QmDeviceMode::PSMStateOn)
                setPSMState (false);
        }
    }
}

bool
BatteryBusinessLogic::PSMValue ()
{
    SYS_DEBUG ("");
    return m_DeviceMode->getPSMState () == QmDeviceMode::PSMStateOn;
}

/*!
 * \param toggle true to go to power save mode, false to leave it
 *
 * Enters or leaves the power saving mode. This function will also disable the
 * auto power save mode.
 */
void
BatteryBusinessLogic::togglePSM (
        bool toggle)
{
    SYS_DEBUG ("");

    setPSMState (toggle);

    //when ever we toggle PSM manually, we turn off the automatic PSM
    m_SystemUIGConf->setValue (
            SystemUIGConf::BatteryPSMAutoKey,
            QVariant (false));
    emit PSMAutoDisabled ();
}

void
BatteryBusinessLogic::togglePSMAuto (
        bool toggle)
{
    SYS_DEBUG ("*** toggle = %s", SYS_BOOL (toggle));

    m_SystemUIGConf->setValue (
            SystemUIGConf::BatteryPSMAutoKey,
            QVariant (toggle));
    if (toggle) // if we turn on the Auto PSM, we must check the threshold
        checkPSMThreshold ();
    else { // if we turn off the Auto PSM, we must disable the PSM in all cases
        if (m_DeviceMode->getPSMState () == QmDeviceMode::PSMStateOn)
            setPSMState (false);
    }

}

QStringList
BatteryBusinessLogic::remainingTimeValues ()
{
    QStringList values;

    if (m_Battery->getChargingState () == QmBattery::StateCharging)
        values << qtTrId (ChargingText) << qtTrId (ChargingText);
    else {
        QmDeviceMode::PSMState state = m_DeviceMode->getPSMState ();
        QmBattery::RemainingTimeMode mode;
        switch (state) {
            case QmDeviceMode::PSMStateOn:
                mode = QmBattery::PowersaveMode;
                break;

            case QmDeviceMode::PSMStateOff:
            default:
                mode = QmBattery::NormalMode;
                break;
                // This was the default...
                //return QStringList () << "N/A" << "N/A";
        }
        values << QString ("%1").arg (m_Battery->getRemainingTalkTime (mode) / 60)
            << QString ("%1").arg (m_Battery->getRemainingIdleTime (mode) / 60);
    }

    #if 0
    int n = 0;
    foreach (QString thisone, values) {
        SYS_DEBUG ("values[%d] = %s", n, SYS_STR (values[n]));
        ++n;
    }
    #endif

    return values;
}

void
BatteryBusinessLogic::utiliseLED (
        bool activate,
        const QString &pattern)
{
    if (activate)
        m_Led->activate (pattern);
    else
        m_Led->deactivate (pattern);
}

void
BatteryBusinessLogic::setPSMThreshold (
        const QString &threshold)
{
    SYS_DEBUG ("*** threshold = '%s'", SYS_STR (threshold));
    m_SystemUIGConf->setValue (
            SystemUIGConf::BatteryPSMThresholdKey,
            QVariant (m_PSMThresholds.indexOf (threshold)));
    checkPSMThreshold ();
}

QVariant
BatteryBusinessLogic::GConfItemValue (
        SystemUIGConf::GConfKey key)
{
    return m_SystemUIGConf->value (key);
}

/*
 * Returns a 5 elements long list of possible PSM threshold value list. 
 */
QStringList
BatteryBusinessLogic::PSMThresholdValues ()
{
    return m_PSMThresholds;
}

/*
 * Returns the string value of the threshold stored in the GConf database. The 
 * threshold value is saved as an index for the possible threshold values that
 * is a list of strings.
 */
QString
BatteryBusinessLogic::PSMThresholdValue ()
{
    return m_PSMThresholds.at (m_SystemUIGConf->value (
                SystemUIGConf::BatteryPSMThresholdKey).toInt ());
}

void
BatteryBusinessLogic::batteryStatus ()
{
    QmBattery::ChargingState state;

    SYS_DEBUG ("What is the state now?");

    state = m_Battery->getChargingState ();
    SYS_DEBUG ("*** state = %d", (int) state);

    switch (state) {
        case QmBattery::StateCharging:
            emit batteryCharging (animationRate (m_Battery->getChargerType ()));
            break;

        case QmBattery::StateNotCharging:
        case QmBattery::StateChargingFailed:
            emit batteryNotCharging ();
            break;
        default:
            break;
    }
}

int
BatteryBusinessLogic::animationRate (
        QmBattery::ChargerType type)
{
    int rate = -1;
    SYS_DEBUG ("");

    switch (type) {
        case QmBattery::USB_500mA:
            rate = ChargingAnimationRateUSB;
            break;

        case QmBattery::Wall:
            rate = ChargingAnimationRateWall;
            break;

        default: // QmBattery::USB_100mA etc.
            break;
    }

    return rate;
}

/*!
 * \param on true for entering PSM mode, false to leave it
 */
void
BatteryBusinessLogic::setPSMState (
        bool     on)
{
    if (on) {
        if (!m_DeviceMode->setPSMState (QmDeviceMode::PSMStateOn)) {
            SYS_WARNING ("Entering power save mode failed.");
        }
    } else {
        if (!m_DeviceMode->setPSMState (QmDeviceMode::PSMStateOff)) {
            SYS_WARNING ("Leaving power save mode failed.");
        }
    }
}

