TEMPLATE = app
TARGET = browser
QT += webkit network

CONFIG += qt warn_on

FORMS += \
    cookies.ui \
    cookiesexceptions.ui \
    downloaditem.ui \
    downloads.ui \
    history.ui \
    passworddialog.ui \
    settings.ui

HEADERS += \
    browsermainwindow.h \
    browsertab.h \
    browsertabproxy.h \
    chasewidget.h \
    cookiejar.h \
    downloadmanager.h \
    edittableview.h \
    edittreeview.h \
    history.h \
    searchlineedit.h \
    sourcecodeview.h \
    squeezelabel.h \
    settings.h \
    util.h \
    toolbarsearch.h

SOURCES += \
    browsermainwindow.cpp \
    browsertab.cpp \
    browsertabproxy.cpp \
    chasewidget.cpp \
    cookiejar.cpp \
    edittableview.cpp \
    edittreeview.cpp \
    searchlineedit.cpp \
    history.cpp \
    settings.cpp \
    sourcecodeview.cpp \
    squeezelabel.cpp \
    toolbarsearch.cpp \
    util.cpp \
    downloadmanager.cpp \
    main.cpp

RESOURCES += images/images.qrc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

win32 {
   RC_FILE      = browser.rc
}

mac {
    ICON = browser.icns
    QMAKE_INFO_PLIST = Info_mac.plist
    TARGET = Browser
}

# install
target.path = $$[QT_INSTALL_DEMOS]/browser
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.html *.doc images
sources.path = $$[QT_INSTALL_DEMOS]/browser
INSTALLS += target sources

