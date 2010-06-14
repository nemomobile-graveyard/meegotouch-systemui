#ifndef LOWBATTERYNOTIFIER_STUB
#define LOWBATTERYNOTIFIER_STUB

#include "batterybusinesslogic.h"
#include <stubbase.h>


// 1. DECLARE STUB
// FIXME - stubgen is not yet finished
class LowBatteryNotifierStub : public StubBase {
  public:
  virtual void LowBatteryNotifierConstructor(QObject *parent);
  virtual void LowBatteryNotifierDestructor();
  virtual void showLowBatteryNotification();
#ifdef HAVE_QMSYSTEM
  virtual void displayStateChanged(Maemo::QmDisplayState::DisplayState state);
#endif
}; 

// 2. IMPLEMENT STUB
void LowBatteryNotifierStub::LowBatteryNotifierConstructor(QObject *parent) {
  Q_UNUSED(parent);

}
void LowBatteryNotifierStub::LowBatteryNotifierDestructor() {

}
void LowBatteryNotifierStub::showLowBatteryNotification() {
  stubMethodEntered("showLowBatteryNotification");
}

#ifdef HAVE_QMSYSTEM
void LowBatteryNotifierStub::displayStateChanged(Maemo::QmDisplayState::DisplayState state) {
  QList<ParameterBase*> params;
  params.append( new Parameter<Maemo::QmDisplayState::DisplayState >(state));
  stubMethodEntered("displayStateChanged",params);
}
#endif



// 3. CREATE A STUB INSTANCE
LowBatteryNotifierStub gDefaultLowBatteryNotifierStub;
LowBatteryNotifierStub* gLowBatteryNotifierStub = &gDefaultLowBatteryNotifierStub;


// 4. CREATE A PROXY WHICH CALLS THE STUB
LowBatteryNotifier::LowBatteryNotifier(QObject *parent) {
  gLowBatteryNotifierStub->LowBatteryNotifierConstructor(parent);
}

LowBatteryNotifier::~LowBatteryNotifier() {
  gLowBatteryNotifierStub->LowBatteryNotifierDestructor();
}

void LowBatteryNotifier::showLowBatteryNotification() {
  gLowBatteryNotifierStub->showLowBatteryNotification();
}

#ifdef HAVE_QMSYSTEM
void LowBatteryNotifier::displayStateChanged(Maemo::QmDisplayState::DisplayState state) {
  gLowBatteryNotifierStub->displayStateChanged(state);
}
#endif


#endif
