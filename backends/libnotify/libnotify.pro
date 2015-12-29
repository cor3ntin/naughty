TARGET = libnotifybackend
TEMPLATE = lib
QT += widgets

include(../backends.pri)

HEADERS += \
    libnotifynotifications.h

SOURCES += \
    libnotifynotifications.cpp

CONFIG += link_pkgconfig
PKGCONFIG += libnotify

