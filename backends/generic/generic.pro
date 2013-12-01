TARGET = genericbackend
TEMPLATE = lib
QT += plugins

include(../backends.pri)

SOURCES += \
    genericnotifications.cpp \
    genericnotificationbox.cpp


HEADERS += \
    genericnotifications.h \
    genericnotificationbox.h
