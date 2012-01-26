#include "keysniffer.h"
#include <unistd.h>

keySniffer::keySniffer() 
: mThreadValid(TRUE),
  mPollTimer(new QTimer())
{
  mD = XOpenDisplay(":0");
  if (mD == NULL)
  {
    //fprintf(stderr, "Cannot open display...\n");
    exit(10);
  }
  connect(mPollTimer, SIGNAL(timeout()), this, SLOT(update()));
  mPollTimer->start(POLLINGTIME);  
}

void keySniffer::run()
{
  XEvent xev;
  bool event_found = false;
  while(mThreadValid) 
  {
    mMutex.lock();
    event_found = XCheckMaskEvent(mD, KeyPressMask, &xev);
    mMutex.unlock();
    
    if (!event_found)
    {
        // Ensure that we are not too busy :)
        usleep(100000);
        continue;
    }
    
    if(xev.type == KeyPress){
      // Scancode to keycode thus -8
      if (xev.xkey.keycode >= 8){
        emit keyPress(xev.xkey.keycode-8, xev.type);
      }
    }
  }
}

void keySniffer::sniff_all_windows(Window root, long type)
{
  Window parent, *children;
  uint nchildren;

  // http://linux.die.net/man/3/xquerytreek
  if (XQueryTree(mD, root, &root, &parent, &children, &nchildren) == 0)
    return;

  // Window has no children to check.
  if (nchildren == 0)
    return;

  // Check the window itself
  XSelectInput(mD, root, type);

  for(int i=0; i < nchildren; i++)
  {
     XSelectInput(mD, children[i], type);
     sniff_all_windows(children[i], type);
  }

  XFree((char *)children);
}

void keySniffer::quit()
{
  mThreadValid = FALSE;
}

void keySniffer::update()
{
  mMutex.lock();
  sniff_all_windows(DefaultRootWindow(mD), KeyPressMask);
  mMutex.unlock();
}
