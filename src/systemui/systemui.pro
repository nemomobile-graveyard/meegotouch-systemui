include(../coverage.pri)
include(../../mconfig.pri)

MOC_DIR = .moc
M_MGEN_OUTDIR = .gen
OBJECTS_DIR = .obj
QMAKE_LFLAGS_RPATH = -Wl
QMAKE_LFLAGS = -Wl,--as-needed

TEMPLATE = app
TARGET = sysuid
target.path = /usr/bin
CONFIG += meegotouch \
          link_pkgconfig

QT += dbus gui meegographicssystemhelper

QMAKE_LIBDIR += ../../lib
LIBS += -lX11 -lXdamage -lmeegotouchviews

SYSTEMUI_SOURCE_DIR = .

contains(cov, true) {
    message("Coverage options enabled")
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}

HEADERS +=                              \
    closeeventeater.h                   \
    sysuid.h                            \
    contextframeworkcontext.h           \
    applicationcontext.h                \
    x11wrapper.h                        \
    xeventlistener.h                    \
    xeventlistenerwidget.h              \
    diskspacenotifier.h

SOURCES +=                              \
    main.cpp                            \
    closeeventeater.cpp                 \
    sysuid.cpp                          \
    contextframeworkcontext.cpp         \
    x11wrapper.cpp                      \
    xeventlistener.cpp                  \
    xeventlistenerwidget.cpp            \
    diskspacenotifier.cpp

include(statusindicatormenu/statusindicatormenu.pri)
include(statusarea/statusarea.pri)
include(../../localconfig.pri)

contains(DEFINES, HAVE_CONTEXTSUBSCRIBER) {
    PKGCONFIG += contextsubscriber-1.0
}

contains(DEFINES, HAVE_QMSYSTEM) {
    PKGCONFIG += qmsystem2
}

PKGCONFIG += \
    xcomposite \
    xfixes

dbus_policy.files += systemui.conf
dbus_policy.path = $$(DEBIAN_DESTDIR)/etc/dbus-1/system.d

INSTALLS += target \
            dbus_policy
