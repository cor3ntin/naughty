import qbs

AnkamaQtPlugin {
    name:"generic"

    pluginCategory : "desktopnotifications"

    Depends { name: "cpp" }
    Depends { name: "Qt.widgets" }
    Depends { name: "naughty" }

    Group {
        name : "Sources"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
}
