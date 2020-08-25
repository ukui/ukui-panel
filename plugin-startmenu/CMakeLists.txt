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

install(FILES
    img/startmenu-pad.png
    DESTINATION "${PACKAGE_DATA_DIR}/plugin-startmenu/img"
    COMPONENT Runtime
)
BUILD_UKUI_PLUGIN(${PLUGIN})
