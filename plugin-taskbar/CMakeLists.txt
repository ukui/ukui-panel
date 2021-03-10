set(PLUGIN "taskbar")

set(HEADERS
    ukuitaskbar.h
    ukuitaskbutton.h
    ukuitaskbarplugin.h
    ukuitaskgroup.h
    ukuigrouppopup.h
    ukuitaskwidget.h
    ukuitaskclosebutton.h
)

set(SOURCES
    ukuitaskbar.cpp
    ukuitaskbutton.cpp
    ukuitaskbarplugin.cpp
    ukuitaskgroup.cpp
    ukuigrouppopup.cpp
    ukuitaskwidget.cpp
    ukuitaskclosebutton.cpp
)

find_package(X11 REQUIRED)

set(LIBRARIES
    ${X11_LIBRARIES}
    Qt5X11Extras
)

find_package(PkgConfig)
pkg_check_modules(GIOUNIX2 REQUIRED gio-unix-2.0)
pkg_check_modules(GLIB2 REQUIRED glib-2.0 gio-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})
#for <QDBusInterface>
include_directories(${_Qt5DBus_OWN_INCLUDE_DIRS})
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
