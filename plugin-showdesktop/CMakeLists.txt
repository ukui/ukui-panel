set(PLUGIN "showdesktop")

set(HEADERS
    showdesktop.h
)

set(SOURCES
    showdesktop.cpp
)
install(FILES
    resources/deskdown.svg   resources/deskup.png
    DESTINATION "/usr/share/ukui-panel/panel/img/"
    COMPONENT Runtime
)

BUILD_UKUI_PLUGIN(${PLUGIN})
