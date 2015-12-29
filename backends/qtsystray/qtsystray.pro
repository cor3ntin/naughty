TARGET = qtsystraybackend
TEMPLATE = lib
QT += widgets

include(../backends.pri)

HEADERS += \
    qtsystraynotifications.h

SOURCES += \
    qtsystraynotifications.cpp

