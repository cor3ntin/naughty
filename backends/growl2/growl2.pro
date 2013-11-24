QT       += core

TARGET = growl2
TEMPLATE = lib
CONFIG += plugin

include(../backends.pri)

OBJECTIVE_SOURCES +=  growlnotifications.mm

HEADERS += growlnotifications.h
HEADERS += Growl/Growl.h Growl/GrowlApplicationBridge.h Growl/GrowlDefines.h


INSTALLS += target

LIBS += -framework Foundation



