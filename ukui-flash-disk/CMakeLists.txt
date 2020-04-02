cmake_minimum_required(VERSION 3.1.0)

project(ukui-flash-disk)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(PkgConfig REQUIRED)


pkg_check_modules(PEONY REQUIRED peony)
pkg_check_modules(BLKID REQUIRED blkid)
message(${PEONY_INCLUDE_DIRS})
message(${PEONY_LIBRARIES})

include_directories(${PEONY_INCLUDE_DIRS})
include_directories(${BLKID_INCLUDE_DIRS})
add_executable(ukui-flash-disk
    ukui-flash-disk.qrc
    disk-resources/ukui-flash-disk_zh_CN.ts
    main.cpp
    mainwindow.cpp
    qclickwidget.cpp
    UnionVariable.cpp
    UnionVariable.h
    ejectInterface.cpp
    ejectInterface.h
    clickLabel.h
    clickLabel.cpp
        mainwindow.ui
        mainwindow.h
        qclickwidget.h

)

add_definitions(-DQT_NO_KEYWORDS)

target_link_libraries(${PROJECT_NAME}  Qt5::Widgets Qt5::DBus ${PEONY_LIBRARIES} ${BLKID_LIBRARIES} )

install(TARGETS ukui-flash-disk DESTINATION bin)

install(FILES
    disk-resources/ukui-flash-disk.desktop
    DESTINATION "/etc/xdg/autostart/"
    COMPONENT Runtime
)

install(FILES
    ukui-flash-disk.qrc disk-resources/ukui-flash-disk_zh_CN.qm
    DESTINATION "/usr/share/ukui/ukui-panel/"
    COMPONENT Runtime
)
