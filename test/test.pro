include(../notification_library.pri)

QT += core gui

TARGET = test

TEMPLATE = app


SOURCES += main.cpp

LIBS += -L$$DESTDIR -lnotification_library

RESOURCES += \
    icon.qrc
