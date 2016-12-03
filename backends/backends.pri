include(../notification_library.pri)
CONFIG += plugin dll include_source_dir
mac:DESTDIR = $$DESTDIR/transition.app/Contents/PlugIns/desktopnotifications/
else:DESTDIR = $$DESTDIR/plugins/desktopnotifications

LIBS += -L$$PROJECTS_BUILD_DIR/bin/ -lnaughty

DEFINES += NAUGHTY_IMPORT

INCLUDEPATH += $$PWD
