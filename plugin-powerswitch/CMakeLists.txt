set(PLUGIN "powerswitch")

set(HEADERS
    powerswitch.h
)

set(SOURCES
    powerswitch.cpp
)

set(UIS
)
set(LIBRARIES
    Qt5Xdg
)

install(FILES
    img/powerswitch.svg
    DESTINATION "${PACKAGE_DATA_DIR}/plugin-powerswitch/img"
    COMPONENT Runtime
)
BUILD_UKUI_PLUGIN(${PLUGIN})
