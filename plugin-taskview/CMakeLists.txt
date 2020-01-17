set(PLUGIN "taskview")

set(HEADERS
    taskview.h
)

set(SOURCES
    taskview.cpp
)

set(UIS
)
set(LIBRARIES
    Qt5Xdg
)

install(FILES
    img/taskview.svg
    DESTINATION "/usr/share/ukui-panel/plugin-taskview/img"
    COMPONENT Runtime
)
BUILD_UKUI_PLUGIN(${PLUGIN})
