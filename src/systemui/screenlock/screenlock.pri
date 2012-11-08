include(../../mconfig.pri)

system(qdbusxml2cpp screenlockbusinesslogic.xml -a screenlockbusinesslogicadaptor -c ScreenLockBusinessLogicAdaptor -l ScreenLockBusinessLogic -i screenlockbusinesslogic.h)

INCLUDEPATH += screenlock statusarea

HEADERS += \
    screenlock/screenlockbusinesslogic.h \
    screenlock/screenlockbusinesslogicadaptor.h \
    screenlock/eventeater.h

SOURCES += \
    screenlock/screenlockbusinesslogic.cpp \
    screenlock/screenlockbusinesslogicadaptor.cpp \
    screenlock/eventeater.cpp
