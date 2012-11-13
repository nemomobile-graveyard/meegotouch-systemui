include(../coverage.pri)
include(../common_top.pri)
TARGET = ut_volumebarlogic
 
INCLUDEPATH += $$ROOTSRCDIR/extensions/volume

CONFIG += link_pkgconfig
PKGCONFIG += dbus-1 dbus-glib-1 libresourceqt1 qmsystem2

HEADERS += \
    ut_volumebarlogic.h \
    $$ROOTSRCDIR/extensions/volume/volumebarlogic.h \
    $$ROOTSRCDIR/extensions/volume/volumebarwindow.h \
    $$ROOTSRCDIR/extensions/volume/keysniffer.h \
    $$SRCDIR/closeeventeater.h

SOURCES += \
    ut_volumebarlogic.cpp \
    $$ROOTSRCDIR/extensions/volume/keysniffer.cpp \
    $$ROOTSRCDIR/extensions/volume/volumebarlogic.cpp \
    $$STUBSDIR/stubbase.cpp

LIBS += -lX11

include(../common_bot.pri)
