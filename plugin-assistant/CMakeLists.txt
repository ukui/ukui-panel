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
    img/search.png
    DESTINATION "${PACKAGE_DATA_DIR}/plugin-assistant/img"
    COMPONENT Runtime
)
BUILD_UKUI_PLUGIN(${PLUGIN})
