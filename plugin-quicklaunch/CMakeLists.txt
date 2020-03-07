set(PLUGIN "quicklaunch")

set(HEADERS
    ukuiquicklaunchplugin.h
    ukuiquicklaunch.h
    quicklaunchbutton.h
    quicklaunchaction.h
    ../panel/customstyle.h
)

set(SOURCES
    ukuiquicklaunchplugin.cpp
    ukuiquicklaunch.cpp
    quicklaunchbutton.cpp
    quicklaunchaction.cpp
    ../panel/customstyle.cpp
)

set(LIBRARIES
    Qt5Xdg
)

include_directories(
    ${UKUI_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/../panel"
)

BUILD_UKUI_PLUGIN(${PLUGIN})
