TARGET = qtsystraybackend
TEMPLATE = lib
QT += plugins

include(../backends.pri)

HEADERS += \
    qtsystraynotifications.h

SOURCES += \
    qtsystraynotifications.cpp

