import qbs
import qbs.TextFile

Project {
    references : [
        "src/src.qbs",
        "backends/generic/generic.qbs",
        "backends/growl2/growl2.qbs",
        "backends/libnotify/libnotify.qbs",
        "backends/osx/osx.qbs",
        "backends/qtsystray/qtsystray.qbs"
    ]
}
