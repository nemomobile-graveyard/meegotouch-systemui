#include "pulseaudio_thread.h"

pulseaudio_thread::pulseaudio_thread(QObject *parent)
: QThread(parent)
{
  mMainLoop = pa_mainloop_new();
  pa_cvolume_init(&mCVolume);
  pa_cvolume_init(&mCChangeVolume);  
  mMainloop_api = pa_mainloop_get_api(mMainLoop);
  mContext = pa_context_new(mMainloop_api,"meego-volume-control");
  pa_context_set_state_callback(mContext,pulseaudio_thread::context_state_callback,this);  
  pa_context_connect(mContext,NULL,(pa_context_flags_t) 0,NULL); 
  //qRegisterMetaType<pa_volume_t>("pa_volume_t");
}

void pulseaudio_thread::run()
{
  int returnValue = 1;
  pa_mainloop_run(mMainLoop,&returnValue);
  pa_mainloop_free(mMainLoop);
}

void pulseaudio_thread::context_state_callback(pa_context *c, void *userdata) 
{
 
  assert(c); 
  switch (pa_context_get_state(c)) {
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;

        case PA_CONTEXT_READY: {
            pulseaudio_thread *pOrigObject = (pulseaudio_thread*)userdata;
            pa_context_get_server_info(pOrigObject->mContext,pulseaudio_thread::getDefaultSinkName_callback,userdata);            
            break;
        }

        case PA_CONTEXT_TERMINATED:
            break;

        case PA_CONTEXT_FAILED:
        default:
            break;
    }
}

void pulseaudio_thread::getDefaultSinkName_callback(pa_context *c, const pa_server_info *i, void *userdata)
{
  assert(c);
  assert(i);
  const size_t len = strlen(i->default_sink_name);
  char * tmpSinkName = new char[len + 1];
  strncpy(tmpSinkName, i->default_sink_name, len);
  tmpSinkName[len] = '\0';
  pulseaudio_thread *pOrigObject = (pulseaudio_thread*)userdata;
  pOrigObject->mDefaultSinkName = tmpSinkName;  
  pa_context_get_sink_info_by_name(pOrigObject->mContext,pOrigObject->mDefaultSinkName,getVolume_callback,userdata);
}

void pulseaudio_thread::changeVolume(quint32 value)
{
  if(mCVolume.channels){
    mCChangeVolume = mCVolume;
    pa_cvolume_scale(&mCChangeVolume,calcVolume(value));
    pa_context_set_sink_volume_by_name(mContext,mDefaultSinkName, &mCChangeVolume, setVolume_callback, this);
  }
}

void pulseaudio_thread::getVolume()
{
  pa_context_get_sink_info_by_name(mContext,mDefaultSinkName,getVolume_callback,this);
}

void pulseaudio_thread::quit()
{
  int returnValue = 1;
  if(mMainLoop){
    delete [] mDefaultSinkName;
    pa_context_unref(mContext);
    pa_context_disconnect(mContext);
    pa_signal_done();
    pa_mainloop_quit(mMainLoop,returnValue);
  }
}

void pulseaudio_thread::updateVolume(pa_cvolume *volume)
{
  if(memcmp(&mCVolume,volume,sizeof(pa_cvolume)))
     memcpy(&mCVolume,volume,sizeof(pa_cvolume));
  emit volumeChanged(calcSteps(pa_cvolume_max(&mCVolume)), MAXVOLUMESTEPS);
}

void pulseaudio_thread::getVolume_callback(pa_context *c, const pa_sink_info *i, int eol, void *paobject)
{
  assert(c);
  if (i < 0) {
    return;
  } else if (!i) {
    return;
  }  
  pulseaudio_thread *pOrigObject = (pulseaudio_thread*)paobject;
  pOrigObject->updateVolume((pa_cvolume*)&i->volume);
}

void pulseaudio_thread::setVolume_callback(pa_context *c, int success, void *userdata)
{
  assert(c);
  if(success){
     pulseaudio_thread *pOrigObject = (pulseaudio_thread*)userdata;
     pa_context_get_sink_info_by_name(pOrigObject->mContext,pOrigObject->mDefaultSinkName,getVolume_callback,userdata);
  }
}

quint32 pulseaudio_thread::calcSteps(pa_volume_t value)
{
  if(value > PA_VOLUME_NORM){
    changeVolume(MAXVOLUMESTEPS);
    return MAXVOLUMESTEPS;
  } else {
    return (value ? value/(PA_VOLUME_NORM/MAXVOLUMESTEPS) : 0);
  }
}

pa_volume_t pulseaudio_thread::calcVolume(quint32 value)
{
  if(value){
    return (value < MAXVOLUMESTEPS ? (PA_VOLUME_NORM/MAXVOLUMESTEPS) * value : PA_VOLUME_NORM);
  } else {
    return PA_VOLUME_MUTED; 
  }
}
