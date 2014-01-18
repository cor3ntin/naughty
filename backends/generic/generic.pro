TARGET = genericbackend
TEMPLATE = lib

include(../backends.pri)

SOURCES += \
    genericnotifications.cpp \
    genericnotificationbox.cpp


HEADERS += \
    genericnotifications.h \
    genericnotificationbox.h
