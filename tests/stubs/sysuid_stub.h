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
#ifndef SYSUID_STUB
#define SYSUID_STUB

#include "sysuid.h"
#include <stubbase.h>
#include <MApplicationWindow>

// 1. DECLARE STUB
// FIXME - stubgen is not yet finished
class SysuidStub : public StubBase {
  public:
  virtual void SysuidConstructor(QObject *parent);
  virtual void SysuidDestructor();
  virtual Sysuid * sysuid();
  virtual void loadTranslations();
}; 

// 2. IMPLEMENT STUB
void SysuidStub::SysuidConstructor(QObject *parent) {
    QList<ParameterBase *> params;
    params.append(new Parameter<QObject*>(parent));
    stubMethodEntered("SysuidConstructor", params);
}

void SysuidStub::SysuidDestructor() {
}

Sysuid * SysuidStub::sysuid() {
  stubMethodEntered("sysuid");
  return stubReturnValue<Sysuid *>("sysuid");
}

void SysuidStub::loadTranslations() {
  stubMethodEntered("loadTranslations");
}


// 3. CREATE A STUB INSTANCE
SysuidStub gDefaultSysuidStub;
SysuidStub* gSysuidStub = &gDefaultSysuidStub;


// 4. CREATE A PROXY WHICH CALLS THE STUB
Sysuid::Sysuid(QObject* parent) :
    batteryBusinessLogic (0), shutdownBusinessLogic (0),
    usbUi (0), statusAreaRenderer (0), statusIndicatorMenuBusinessLogic (0),
    volumeExtensionArea (0)
{
  gSysuidStub->SysuidConstructor(parent);
}

Sysuid::~Sysuid() {
  gSysuidStub->SysuidDestructor();
}

Sysuid * Sysuid::instance() {
  return gSysuidStub->sysuid();
}

void Sysuid::loadTranslations() {
  gSysuidStub->loadTranslations();
}

#endif
