set(PLUGIN "segmentation")

set(HEADERS
    segmentation.h
)

set(SOURCES
    segmentation.cpp
)

install(FILES
    img/segmentation.svg img/segmentationh.svg
    DESTINATION "${PACKAGE_DATA_DIR}/plugin-segmentation/img"
    COMPONENT Runtime
)
BUILD_UKUI_PLUGIN(${PLUGIN})
