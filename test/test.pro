include(../notification_library.pri)

QT += core gui

TARGET = test

TEMPLATE = app


SOURCES += main.cpp

LIBS += -L$$DESTDIR -lnaughty

RESOURCES += \
    icon.qrc

DEFINES += NAUGHTY_IMPORT
