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
    cpp.linkerFlags : pkgConfig.libs

    Group {
        name : "Sources"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
}
