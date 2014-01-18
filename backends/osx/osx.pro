QT       += core

TARGET = osx
TEMPLATE = lib
CONFIG += plugin

include(../backends.pri)

OBJECTIVE_SOURCES +=  osxnotifications.mm

HEADERS += osxnotifications.h


LIBS += -framework Foundation
