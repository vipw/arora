INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
  arorawebplugin.h \
  webpluginfactory.h

SOURCES += \
  arorawebplugin.cpp \
  webpluginfactory.cpp

include(rss/rss.pri)
include(clicktoflash/clicktoflash.pri)

