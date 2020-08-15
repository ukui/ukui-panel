set(PLUGIN "taskbar")

set(HEADERS
    ukuitaskbar.h
    ukuitaskbutton.h
    ukuitaskbarplugin.h
    ukuitaskgroup.h
    ukuigrouppopup.h
    ukuitaskwidget.h
    ukuitaskclosebutton.h
	ukuitaskbaricon.h
)

set(SOURCES
    ukuitaskbar.cpp
    ukuitaskbutton.cpp
    ukuitaskbarplugin.cpp
    ukuitaskgroup.cpp
    ukuigrouppopup.cpp
    ukuitaskwidget.cpp
    ukuitaskclosebutton.cpp
    ukuitaskbaricon.cpp
)

find_package(X11 REQUIRED)

set(LIBRARIES
    Qt5Xdg
    ${X11_LIBRARIES}
    Qt5X11Extras
)

pkg_check_modules(GLIB2 REQUIRED glib-2.0 gio-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})

BUILD_UKUI_PLUGIN(${PLUGIN})
