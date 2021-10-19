set(PLUGIN "showdesktop")

set(HEADERS
    showdesktop.h
)

set(SOURCES
    showdesktop.cpp
)

include(../cmake/UkuiPluginTranslationTs.cmake)
ukui_plugin_translate_ts(${PLUGIN})

BUILD_UKUI_PLUGIN(${PLUGIN})
