cmake_minimum_required(VERSION 3.1.0)

project(sni-daemon)

#判断编译器类型,如果是gcc编译器,则在编译选项中加入c++11支持
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(PkgConfig REQUIRED)

add_executable(sni-daemon
        main.cpp
        snideamo.cpp
        statusnotifieritem_interface.cpp

        snideamo.h
        statusnotifieritem_interface.h
        systemtraytypedefs.h
)

target_link_libraries(sni-daemon
    Qt5::Widgets
    Qt5::DBus
    )

add_definitions(-DQT_MESSAGELOGCONTEXT)

install(TARGETS sni-daemon DESTINATION bin)

install(FILES
    resources/sni-daemon.desktop
    DESTINATION "/etc/xdg/autostart/"
    COMPONENT Runtime
)
