set(PLUGIN "nightmode")

set(HEADERS
    nightmode.h
)

set(SOURCES
    nightmode.cpp
)

set(UIS
)

find_package(PkgConfig)
pkg_check_modules(Gsetting REQUIRED gsettings-qt)
include_directories(${Gsetting_INCLUDE_DIRS})

set(LIBRARIES
    Qt5Xdg
    ${Gsetting_LIBRARIES}
)

BUILD_UKUI_PLUGIN(${PLUGIN})
