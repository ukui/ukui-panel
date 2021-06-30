set(PLUGIN "startbar")

set(HEADERS
    startbar.h
    startmenu_button.h
    taskview_button.h
)

set(SOURCES
    startbar.cpp
    startmenu_button.cpp
    taskview_button.cpp
)

set(UIS
)

set(LIBRARIES
)

install(FILES
    img/search.png
    DESTINATION "${PACKAGE_DATA_DIR}/plugin-assistant/img"
    COMPONENT Runtime
)
BUILD_UKUI_PLUGIN(${PLUGIN})
