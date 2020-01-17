set(PLUGIN "showdesktop")

set(HEADERS
    showdesktop.h
)

set(SOURCES
    showdesktop.cpp
)

set(LIBRARIES
    ${LIBRARIES}
    Qt5Xdg
)

BUILD_UKUI_PLUGIN(${PLUGIN})
