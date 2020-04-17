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

pkg_check_modules(GLIB2 REQUIRED glib-2.0 gio-2.0)

include_directories(${GLIB2_INCLUDE_DIRS})
add_executable(ukui-flash-disk
    ukui-flash-disk.qrc
    disk-resources/ukui-flash-disk_zh_CN.ts
    main.cpp
    mainwindow.cpp
    qclickwidget.cpp
    qclickwidget.h
    UnionVariable.cpp
    UnionVariable.h
    ejectInterface.cpp
    ejectInterface.h
    clickLabel.h
    clickLabel.cpp
    MainController.h
    MainController.cpp
    MacroFile.h
        mainwindow.ui
        mainwindow.h

)

add_definitions(-DQT_NO_KEYWORDS)

target_link_libraries(${PROJECT_NAME}  Qt5::Widgets Qt5::DBus ${GLIB2_LIBRARIES} )

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
