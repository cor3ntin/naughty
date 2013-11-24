include(../notification_library.pri)
CONFIG += plugin dll include_source_dir
DESTDIR = $$DESTDIR/plugins/desktopnotifications
LIBS += -L$$PROJECTS_BUILD_DIR/bin/ -lnotification_library
