set(PLUGIN "taskbar")

set(HEADERS
    ukuitaskbar.h
    ukuitaskbutton.h
    ukuitaskbarconfiguration.h
    ukuitaskbarplugin.h
    ukuitaskgroup.h
    ukuigrouppopup.h
)

set(SOURCES
    ukuitaskbar.cpp
    ukuitaskbutton.cpp
    ukuitaskbarconfiguration.cpp
    ukuitaskbarplugin.cpp
    ukuitaskgroup.cpp
    ukuigrouppopup.cpp
)

set(UIS
    ukuitaskbarconfiguration.ui
)

set(LIBRARIES
    Qt5Xdg
    Qt5X11Extras
)
find_package(X11 REQUIRED)
#find_package(PkgConfig)
##configure_file(no_keywords link_pkgconfig)
#pkg_check_modules(TASKBAR_DEPS REQUIRED Qt5X11Extras)
#include_directories(${TASKBAR_DEPS_INCLUDE_DIRS})
#ADD_DEFINITIONS(-DQT_NO_KEYWORDS)
#link_libraries(libQt5X11Extras.so)

BUILD_UKUI_PLUGIN(${PLUGIN})
