cmake_minimum_required(VERSION 3.1.0)

project(ukui-flash-disk)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(ukui-flash-disk
    main.cpp 
    mainwindow.cpp 
    qclickwidget.cpp
	mainwindow.ui
	mainwindow.h
	qclickwidget.h
)

target_link_libraries(ukui-flash-disk Qt5::Widgets)
install(TARGETS ukui-flash-disk DESTINATION bin)
install(FILES
    ukui-flash-disk.desktop
    DESTINATION "/etc/xdg/autostart/"
    COMPONENT Runtime
)


