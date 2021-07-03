set(PLUGIN "taskview")

set(HEADERS
    taskview.h
)

set(SOURCES
    taskview.cpp
)

include_directories(${_Qt5DBus_OWN_INCLUDE_DIRS})

include(../cmake/UkuiPluginTranslationTs.cmake)
ukui_plugin_translate_ts(${PLUGIN})

install(FILES
    img/taskview.svg
    DESTINATION "/usr/share/ukui-panel/plugin-taskview/img"
    COMPONENT Runtime
)
BUILD_UKUI_PLUGIN(${PLUGIN})
