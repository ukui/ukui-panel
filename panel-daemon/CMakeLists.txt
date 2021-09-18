cmake_minimum_required(VERSION 3.1.0)

project(panel-daemon)

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

pkg_check_modules(GLIB2 REQUIRED glib-2.0 gio-2.0 udisks2)
pkg_check_modules(QGS REQUIRED gsettings-qt)


include_directories(${GLIB2_INCLUDE_DIRS})
include_directories(${QGS_INCLUDE_DIRS})
add_executable(panel-daemon
    jsonwathcer/jsonwacther.cpp
    jsonwathcer/jsonwacther.h
    filewatcher/filewatcher.cpp
    filewatcher/filewatcher.h
    convert-desktop-windowid/convertdesktoptowinid.cpp
    convert-desktop-windowid/convertdesktoptowinid.h
    dbus-server/server.cpp
    dbus-server/server.h
    dbus-server/dbus-adaptor.cpp
    dbus-server/dbus-adaptor.h
    datewather/datewatcher.cpp
    datewather/datewatcher.h
    watchermanager.cpp
    watchermanager.h
    main.cpp
)


add_definitions(-DQT_NO_KEYWORDS)
add_definitions(-DQT_MESSAGELOGCONTEXT)

target_link_libraries(${PROJECT_NAME}  Qt5::Widgets Qt5::DBus Qt5::Network ${GLIB2_LIBRARIES} ${QGS_LIBRARIES} KF5::WindowSystem)

install(TARGETS panel-daemon DESTINATION bin)

