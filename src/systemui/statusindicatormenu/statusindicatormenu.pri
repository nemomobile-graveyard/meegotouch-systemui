include(../../mconfig.pri)

INCLUDEPATH += statusindicatormenu

SOURCES += \
    statusindicatormenu/statusindicatormenubusinesslogic.cpp \
    statusindicatormenu/statusindicatormenuwindow.cpp \
    statusindicatormenu/statusindicatormenuadaptor.cpp \
    statusindicatormenu/statusindicatormenu.cpp \
    statusindicatormenu/statusindicatormenudropdownview.cpp \
    statusindicatormenu/statusindicatormenuverticalview.cpp

HEADERS += \
    statusindicatormenu/statusindicatormenubusinesslogic.h \
    statusindicatormenu/statusindicatormenuwindow.h \
    statusindicatormenu/statusindicatormenuadaptor.h \
    statusindicatormenu/statusindicatormenustyle.h \
    statusindicatormenu/statusindicatormenu.h \
    statusindicatormenu/statusindicatormenudropdownview.h \
    statusindicatormenu/statusindicatormenuverticalview.h

STYLE_HEADERS += statusindicatormenu/statusindicatormenustyle.h

headers.path = /usr/include/system-ui
headers.files += statusindicatormenu/mstatusindicatormenuextensioninterface.h statusindicatormenu/MStatusIndicatorMenuExtensionInterface \
                statusindicatormenu/mstatusindicatormenuinterface.h
prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
prf.files = statusindicatormenu/system-ui.prf

INSTALLS += headers prf
