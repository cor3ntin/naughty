import qbs

AnkamaQtPlugin {
    name:"growl2"

    pluginCategory : "desktopnotifications"

    Depends { name: "cpp" }
    Depends { name: "Qt.widgets" }
    Depends { name: "naughty" }

    Group {
        name : "Sources"
        files: [
            "*.cpp",
            "**/*.h",
            "*.mm"
        ]
    }
    condition: qbs.targetOS.contains("osx")
    cpp.frameworks : ["Foundation"]
    cpp.includePaths : ["."]
}
