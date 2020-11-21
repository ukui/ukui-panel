set(PLUGIN "quicklaunch")

set(HEADERS
    ukuiquicklaunchplugin.h
    ukuiquicklaunch.h
    quicklaunchbutton.h
    quicklaunchaction.h
    json.h
#    ../panel/customstyle.h
)

set(SOURCES
    ukuiquicklaunchplugin.cpp
    ukuiquicklaunch.cpp
    quicklaunchbutton.cpp
    quicklaunchaction.cpp
    json.cpp
#    ../panel/customstyle.cpp
)

find_package(PkgConfig)
pkg_check_modules(GIOUNIX2 REQUIRED gio-unix-2.0)

include_directories(
    ${UKUI_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/../panel"
    ${GIOUNIX2_INCLUDE_DIRS}
)

set(LIBRARIES
    Qt5Xdg
    ${GIOUNIX2_LIBRARIES}
)

BUILD_UKUI_PLUGIN(${PLUGIN})
