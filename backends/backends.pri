include(../notification_library.pri)
CONFIG += plugin dll include_source_dir
DESTDIR = $$DESTDIR/plugins/desktopnotifications
mac:DESTDIR = $$PROJECTS_APPS_PATH/transition.app/Contents/PlugIns/desktopnotifications

LIBS += -L$$PROJECTS_BUILD_DIR/bin/ -lnaughty

DEFINES += NAUGHTY_IMPORT

INCLUDEPATH += $$PWD
