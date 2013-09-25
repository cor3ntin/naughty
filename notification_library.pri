defineReplace(cleanPath) {
    win32:1 ~= s|\\\\|/|g
    contains(1, ^/.*):pfx = /
    else:pfx =
    segs = $$split(1, /)
    out =
    for(seg, segs) {
        equals(seg, ..):out = $$member(out, 0, -2)
        else:!equals(seg, .):out += $$seg
    }
    return($$join(out, /, $$pfx))
}

# For use in custom compilers which just copy files
win32:i_flag = i
defineReplace(stripSrcDir) {
    win32 {
        !contains(1, ^.:.*):1 = $$OUT_PWD/$$1
    } else {
        !contains(1, ^/.*):1 = $$OUT_PWD/$$1
    }
    out = $$cleanPath($$1)
    out ~= s|^$$re_escape($$_PRO_FILE_PWD_/)||$$i_flag
    return($$out)
}

defineReplace(targetPath) {
    return($$replace(1, /, $$QMAKE_DIR_SEP))
}
defineReplace(prependAll) {
    for(a,$$1):result += $$2$${a}$$3
    return($$result)
}



OBJECTS_DIR = .tmp/objs
MOC_DIR = .tmp/moc

DEFINES += QT_NO_KEYWORDS

CONFIG += depend_includepath
win32: CONFIG += rtti
CONFIG -= exceptions

win32-g++:QMAKE_LFLAGS += -static-libgcc -static-libstdc++



PROJECT_ROOT=$$PWD
isEmpty(PROJECTS_BUILD_DIR){
    sub_dir  = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    PROJECTS_BUILD_DIR = $$cleanPath($$OUT_PWD)
    PROJECTS_BUILD_DIR ~= s,$$re_escape($$sub_dir)$,,
}



PROJECT_ROOT = $$PWD
DESTDIR = $$PROJECTS_BUILD_DIR/bin/
INCLUDEPATH += $$PROJECT_ROOT/src
