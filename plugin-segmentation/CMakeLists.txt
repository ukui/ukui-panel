set(PLUGIN "segmentation")

set(HEADERS
    segmentation.h
)

set(SOURCES
    segmentation.cpp
)

set(UIS
)
set(LIBRARIES
    Qt5Xdg
)
install(FILES
    img/segmentation.svg img/segmentationh.svg
    DESTINATION "${PACKAGE_DATA_DIR}/plugin-segmentation/img"
    COMPONENT Runtime
)
set(UKUI_TRANSLATIONS_DAT "/usr/local/share/")
file(GLOB UKUI_TRANSLATIONS_SOURCE ${UKUI_TRANSLATIONS_DAT}ukui/translations/ukui-panel/ukui-panel_zh_CN.qm )
BUILD_UKUI_PLUGIN(${PLUGIN})
