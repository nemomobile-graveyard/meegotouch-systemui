#ifndef PULSEAUDIO_THREAD_H
#define PULSEAUDIO_THREAD_H

#ifndef UINT32_MAX
#define UINT32_MAX  (0xffffffff)
#endif // UINT32_MAX

#include <QThread>

#include <pulse/pulseaudio.h>
#include <pulse/volume.h>

#define MAXVOLUMESTEPS 10

class pulseaudio_thread : public QThread
{
  Q_OBJECT
public:  
  pulseaudio_thread(QObject *parent = 0);
  static void context_state_callback(pa_context *c, void *userdata);
  static void getDefaultSinkName_callback(pa_context *c, const pa_server_info *i, void *sinkname);
  static void getVolume_callback(pa_context *c, const pa_sink_info *i, int eol, void *currentVolume);
  static void setVolume_callback(pa_context *c, int success, void *userdata);    
  void run();
  void updateVolume(pa_cvolume *volume);
public Q_SLOTS:
  void changeVolume(quint32 value);
  void getVolume();
  void quit();
signals:
  void volumeChanged(quint32 value, quint32 maxValue);
private:
  quint32     calcSteps(pa_volume_t value);
  pa_volume_t calcVolume(quint32 value);
  pa_mainloop                 *mMainLoop;
  pa_mainloop_api             *mMainloop_api;
  pa_context                  *mContext;
  char                        *mDefaultSinkName;
  pa_cvolume                  mCVolume;
  pa_cvolume                  mCChangeVolume;
};

#endif // PULSEAUDIO_THREAD_H

