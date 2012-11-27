include(../coverage.pri)
include(../common_top.pri)
TARGET = ut_sysuid
INCLUDEPATH += $$SRCDIR/statusarea $$SRCDIR/statusindicatormenu $$SRCDIR/volumecontrol
INCLUDEPATH += /usr/include/contextsubscriber /usr/include/resource/qt4

PKGCONFIG += dbus-1

# unit test and unit classes
SOURCES += \
    ut_sysuid.cpp \
    $$SRCDIR/sysuid.cpp

# service classes
SOURCES += \
    $$STUBSDIR/stubbase.cpp \
    $$STUBSDIR/systemstatestub.cpp \
    $$STUBSDIR/testcontextitem.cpp \

# unit test and unit classes
HEADERS += \
    ut_sysuid.h \
    $$SRCDIR/shutdownui.h \
    $$SRCDIR/usbui.h \
    $$SRCDIR/shutdownbusinesslogic.h \
    $$SRCDIR/batterybusinesslogic.h \
    $$SRCDIR/sysuid.h \
    $$SRCDIR/contextframeworkcontext.h \
    $$SRCDIR/statusarea/statusarearenderer.h \
    $$SRCDIR/statusarea/statusarearendereradaptor.h \
    $$SRCDIR/statusarea/statusareastyle.h \
    $$SRCDIR/applicationcontext.h \
    $$SRCDIR/statusindicatormenu/statusindicatormenubusinesslogic.h \
    $$SRCDIR/statusindicatormenu/statusindicatormenuadaptor.h \
    $$SRCDIR/x11wrapper.h \
    $$SRCDIR/closeeventeater.h \
    $$SRCDIR/diskspacenotifier.h

# service classes
HEADERS += \
    $$STUBSDIR/stubbase.h \
    $$STUBSDIR/systemstatestub.h \
    $$STUBSDIR/locksstub.h \
    $$STUBSDIR/testcontextitem.h

STYLE_HEADERS += $$SRCDIR/statusarea/statusareastyle.h

include(../common_bot.pri)
