include(../notification_library.pri)
CONFIG += plugin dll include_source_dir
DESTDIR = $$DESTDIR/plugins/desktopnotifications

#win32 {
#CONFIG(debug, debug|release) : LIBS += -L$$PROJECTS_BUILD_DIR/bin/debug/
#else: LIBS += -L$$PROJECTS_BUILD_DIR/bin/release
#}
#else:
LIBS += -L$$PROJECTS_BUILD_DIR/bin/ -lnotification_library

DEFINES += NAUGHTY_IMPORT
