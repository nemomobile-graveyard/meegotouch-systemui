#ifndef VOLUMEBARLOGIC_STUB
#define VOLUMEBARLOGIC_STUB

#include "volumebarlogic.h"
#include <stubbase.h>


// 1. DECLARE STUB
// FIXME - stubgen is not yet finished
class VolumeBarLogicStub : public StubBase {
  public:
  virtual void VolumeBarLogicConstructor(QObject *parent);
  virtual void VolumeBarLogicDestructor();
  virtual quint32 volume();
  virtual quint32 maxVolume();
  virtual void setVolume(pa_volume_t value);
  virtual void stepsUpdated(pa_volume_t value, pa_volume_t maxvalue);
  virtual void hwKeyEvent(uint key, int state);
  virtual void hwKeyResourceAcquired();
  virtual void hwKeyResourceLost();
  virtual void openConnection();
}; 

// 2. IMPLEMENT STUB
void VolumeBarLogicStub::VolumeBarLogicConstructor(QObject *parent) {
  Q_UNUSED(parent);

}
void VolumeBarLogicStub::VolumeBarLogicDestructor() {

}
quint32 VolumeBarLogicStub::volume() {
  stubMethodEntered("volume");
  return stubReturnValue<quint32>("volume");
}

quint32 VolumeBarLogicStub::maxVolume() {
  stubMethodEntered("maxVolume");
  return stubReturnValue<quint32>("maxVolume");
}

void VolumeBarLogicStub::setVolume(pa_volume_t value) {
  QList<ParameterBase*> params;
  params.append( new Parameter<quint32>(value));
  stubMethodEntered("setVolume",params);
}

void VolumeBarLogicStub::stepsUpdated(quint32 value, quint32 maxvalue) {
  QList<ParameterBase*> params;
  params.append( new Parameter<quint32 >(value));
  params.append( new Parameter<quint32 >(maxvalue));
  stubMethodEntered("stepsUpdated",params);
}

void VolumeBarLogicStub::hwKeyEvent(uint key, int state) {
  QList<ParameterBase*> params;
  params.append( new Parameter<uint>(key));
  params.append( new Parameter<int>(state));
  stubMethodEntered("hwKeyEvent",params);
}

void VolumeBarLogicStub::hwKeyResourceAcquired() {
  stubMethodEntered("hwKeyResourceAcquired");
}

void VolumeBarLogicStub::hwKeyResourceLost() {
  stubMethodEntered("hwKeyResourceLost");
}

void VolumeBarLogicStub::openConnection() {
  stubMethodEntered("openConnection");
}

// 3. CREATE A STUB INSTANCE
VolumeBarLogicStub gDefaultVolumeBarLogicStub;
VolumeBarLogicStub* gVolumeBarLogicStub = &gDefaultVolumeBarLogicStub;


// 4. CREATE A PROXY WHICH CALLS THE STUB
VolumeBarLogic::VolumeBarLogic(QObject *parent) {
  gVolumeBarLogicStub->VolumeBarLogicConstructor(parent);
}

VolumeBarLogic::~VolumeBarLogic() {
  gVolumeBarLogicStub->VolumeBarLogicDestructor();
}

quint32 VolumeBarLogic::volume() {
  return gVolumeBarLogicStub->volume();
}

quint32 VolumeBarLogic::maxVolume() {
  return gVolumeBarLogicStub->maxVolume();
}

void VolumeBarLogic::setVolume(pa_volume_t value) {
  gVolumeBarLogicStub->setVolume(value);
}

void VolumeBarLogic::stepsUpdated(pa_volume_t value, pa_volume_t maxvalue) {
  gVolumeBarLogicStub->stepsUpdated(value, maxvalue);
}

void VolumeBarLogic::hwKeyEvent(uint key, int state) {
  gVolumeBarLogicStub->hwKeyEvent(key, state);
}

void VolumeBarLogic::hwKeyResourceAcquired() {
  gVolumeBarLogicStub->hwKeyResourceAcquired();
}

void VolumeBarLogic::hwKeyResourceLost() {
  gVolumeBarLogicStub->hwKeyResourceLost();
}

void VolumeBarLogic::openConnection() {
  gVolumeBarLogicStub->openConnection();
}


#endif
