cmake_minimum_required(VERSION 3.1.0)

project(ukui-calendar)

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
find_package(Qt5LinguistTools ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(KF5WindowSystem REQUIRED)

pkg_check_modules(GLIB2 REQUIRED glib-2.0 gio-2.0 udisks2)
pkg_check_modules(QGS REQUIRED gsettings-qt)


include_directories(${GLIB2_INCLUDE_DIRS})
include_directories(${QGS_INCLUDE_DIRS})
add_executable(ukui-calendar
    lunarcalendarwidget/customstylePushbutton.cpp
    lunarcalendarwidget/frmlunarcalendarwidget.cpp
    lunarcalendarwidget/frmlunarcalendarwidget.ui
    lunarcalendarwidget/lunarcalendarinfo.cpp
    lunarcalendarwidget/lunarcalendaritem.cpp
    lunarcalendarwidget/lunarcalendarmonthitem.cpp
    lunarcalendarwidget/lunarcalendarwidget.cpp
    lunarcalendarwidget/lunarcalendaryearitem.cpp
    lunarcalendarwidget/main.cpp
    lunarcalendarwidget/picturetowhite.cpp
    lunarcalendarwidget/customstylePushbutton.h
    lunarcalendarwidget/frmlunarcalendarwidget.h
    lunarcalendarwidget/lunarcalendarinfo.h
    lunarcalendarwidget/lunarcalendaritem.h
    lunarcalendarwidget/lunarcalendarmonthitem.h
    lunarcalendarwidget/lunarcalendarwidget.h
    lunarcalendarwidget/lunarcalendaryearitem.h
    lunarcalendarwidget/picturetowhite.h
)


add_definitions(-DQT_NO_KEYWORDS)
add_definitions(-DQT_MESSAGELOGCONTEXT)

target_link_libraries(${PROJECT_NAME}  Qt5::Widgets Qt5::DBus Qt5::Network ${GLIB2_LIBRARIES} ${QGS_LIBRARIES} KF5::WindowSystem -lukui-log4qt)

install(TARGETS ukui-calendar DESTINATION bin)

install(FILES
    resources/ukui-calendar.desktop
    DESTINATION "/etc/xdg/autostart/"
    COMPONENT Runtime
)

#install(FILES
#    ukui-flash-disk.qrc
#    DESTINATION "/usr/share/ukui/ukui-panel/"
#    COMPONENT Runtime
#)


# To create a new ts file: lupdate -recursive . -target-language zh_CN -ts translation/ukui-calendar_zh_CN.ts
file(GLOB TS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/translation/ukui-calendar_zh_CN.ts")

# cmake -DUPDATE_TRANSLATIONS=yes
if (UPDATE_TRANSLATIONS)
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_add_translation(QM_FILES ${TS_FILES})
endif()
add_custom_target(translations ALL DEPENDS ${QM_FILES})
#install(FILES ${QM_FILES} DESTINATION "/usr/share/ukui/ukui-panel/")
install(FILES ${QM_FILES} DESTINATION "/usr/share/ukui-panel/panel/resources/")


