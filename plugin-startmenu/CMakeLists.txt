set(PLUGIN "startmenu")

set(HEADERS
    startmenu.h
)

set(SOURCES
    startmenu.cpp
)

set(UIS
)

set(LIBRARIES
)

include(../cmake/UkuiPluginTranslationTs.cmake)
ukui_plugin_translate_ts(${PLUGIN})

install(FILES
    img/startmenu.svg
    DESTINATION "${PACKAGE_DATA_DIR}/plugin-startmenu/img"
    COMPONENT Runtime
)
BUILD_UKUI_PLUGIN(${PLUGIN})
