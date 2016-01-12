import qbs
import qbs.Probes


AnkamaQtPlugin {
    name:"libnotify"

    pluginCategory : "desktopnotifications"

    Depends { name: "cpp" }
    Depends { name: "Qt.widgets" }
    Depends { name: "naughty" }

    Probes.PkgConfigProbe {
        id: pkgConfig
        name: 'libnotify'
    }

    condition: {
        return pkgConfig.found
    }

    cpp.cxxFlags : pkgConfig.cflags
    cpp.dynamicLibraries : ["notify", "gdk_pixbuf-2.0", "gio-2.0", "gobject-2.0", "glib-2.0"]
    Group {
        name : "Sources"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
}
