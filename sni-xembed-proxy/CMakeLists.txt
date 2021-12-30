cmake_minimum_required(VERSION 3.1.0)

project(sni-xembed-proxy)

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
find_package(KF5WindowSystem REQUIRED)
find_package(Qt5X11Extras REQUIRED)

add_executable(sni-xembed-proxy
        main.cpp
        snixembedproxy.cpp
        sniproxy.cpp
        snidbus.cpp
        statusnotifierwatcher_interface.cpp
        ../panel-daemon/convert-desktop-windowid/convertdesktoptowinid.cpp

        snixembedproxy.h
        sniproxy.h
        snidbus.h
        statusnotifierwatcher_interface.h
        systemtraytypedefs.h
        xcbutilss.h
        ../panel-daemon/convert-desktop-windowid/convertdesktoptowinid.h
)

target_link_libraries(sni-xembed-proxy
    Qt5::X11Extras
    Qt5::Widgets
    Qt5::DBus
    KF5::WindowSystem
    xcb
    xcb-util
    xcb-damage
    xcb-image
    xcb-composite
    xcb-shape
    -lukui-log4qt
    -lQt5Xdg
    )

add_definitions(-DQT_NO_KEYWORDS)
add_definitions(-DQT_MESSAGELOGCONTEXT)

install(TARGETS sni-xembed-proxy DESTINATION bin)

install(FILES
    resources/sni-xembed-proxy.desktop
    DESTINATION "/etc/xdg/autostart/"
    COMPONENT Runtime
)
