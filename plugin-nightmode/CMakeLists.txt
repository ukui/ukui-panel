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

BUILD_UKUI_PLUGIN(${PLUGIN})
