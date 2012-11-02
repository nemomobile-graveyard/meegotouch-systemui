include(../coverage.pri)
include(../common_top.pri)
TARGET = ut_sysuid
INCLUDEPATH += $$SRCDIR/statusarea $$LIBNOTIFICATIONSRCDIR $$NOTIFICATIONSRCDIR $$SRCDIR/statusindicatormenu $$SRCDIR/volumecontrol $$SRCDIR/screenlock
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
    $$SRCDIR/screenlock/eventeater.h \
    $$SRCDIR/shutdownbusinesslogic.h \
    $$SRCDIR/batterybusinesslogic.h \
    $$SRCDIR/screenlock/screenlockbusinesslogic.h \
    $$SRCDIR/screenlock/screenlockbusinesslogicadaptor.h \
    $$SRCDIR/sysuid.h \
    $$SRCDIR/contextframeworkcontext.h \
    $$NOTIFICATIONSRCDIR/eventtypestore.h \
    $$LIBNOTIFICATIONSRCDIR/notificationsink.h \
    $$NOTIFICATIONSRCDIR/widgetnotificationsink.h \
    $$NOTIFICATIONSRCDIR/mcompositornotificationsink.h \
    $$NOTIFICATIONSRCDIR/ngfnotificationsink.h \
    $$NOTIFICATIONSRCDIR/notificationstatusindicatorsink.h \
    $$NOTIFICATIONSRCDIR/notificationmanager.h \
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
