import qbs

AnkamaDynamicLibrary {
    name:"naughty"
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.gui" }

    cpp.defines: ['NAUGHTY_EXPORT']


    Group {
        name : "Sources"
        files: ["*.cc", "*.cpp", "*.h"]
    }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths : ['.']
        cpp.defines: ['NAUGHTY_IMPORT']
    }
}
