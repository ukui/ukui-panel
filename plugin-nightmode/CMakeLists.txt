set(PLUGIN "nightmode")

set(HEADERS
    nightmode.h
)

set(SOURCES
    nightmode.cpp
)

set(LIBRARIES
#    Qt5Xdg
    ${Gsetting_LIBRARIES}
)
include_directories(${_Qt5DBus_OWN_INCLUDE_DIRS})
BUILD_UKUI_PLUGIN(${PLUGIN})
