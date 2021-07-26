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

include(../cmake/UkuiPluginTranslationTs.cmake)
ukui_plugin_translate_ts(${PLUGIN})

include_directories(${_Qt5DBus_OWN_INCLUDE_DIRS})
BUILD_UKUI_PLUGIN(${PLUGIN})
