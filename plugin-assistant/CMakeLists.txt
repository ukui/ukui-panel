set(PLUGIN "assistant")

set(HEADERS
    ukuiassistant.h
    searchwindow.h
)

set(SOURCES
    ukuiassistant.cpp
    searchwindow.cpp
)

set(UIS
    searchwindow.ui
)

set(LIBRARIES
)

install(FILES
    img/startmenu.svg
    DESTINATION "${PACKAGE_DATA_DIR}/plugin-startmenu/img"
    COMPONENT Runtime
)
BUILD_UKUI_PLUGIN(${PLUGIN})
