#ifndef KEYSNIFFER_H
#define KEYSNIFFER_H

#include <QThread>
#include <QMutex>
#include <QPointer>
#include <QTimer>

#include <X11/X.h>
#include <X11/Xlib.h>

// Polling time for keysniffer in ms.
#define POLLINGTIME 2000

class keySniffer : public QThread
{
  Q_OBJECT
public:
    keySniffer();    
    void run();
public Q_SLOTS:
    void quit();
signals:
    void keyPress(uint keycode, int event);
private Q_SLOTS:
    void update();
private:
    void sniff_all_windows(Window root, long type);
    
    Display *mD;
    bool    mThreadValid;
    QMutex  mMutex;
    QPointer<QTimer> mPollTimer;
};

#endif // KEYSNIFFER_H

