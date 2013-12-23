include(../notification_library.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = notification_library
TEMPLATE = lib


SOURCES += \
    desktopnotification.cpp

HEADERS  += \
    desktopnotification.h \
    desktopnotificationbackend.h \
    desktopnotification_p.h \
    desktopnotification_global.h

DEFINES += NAUGHTY_EXPORT
