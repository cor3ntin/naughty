TARGET = genericbackend
TEMPLATE = lib
QT += widgets

include(../backends.pri)

SOURCES += \
    genericnotifications.cpp \
    genericnotificationbox.cpp


HEADERS += \
    genericnotifications.h \
    genericnotificationbox.h
