
set(PLUGIN "calendar")
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)

set(HEADERS
    ukuicalendar.h
    ukuiwebviewdialog.h
    lunarcalendarwidget/frmlunarcalendarwidget.h
    lunarcalendarwidget/lunarcalendarinfo.h
    lunarcalendarwidget/lunarcalendaritem.h
    lunarcalendarwidget/lunarcalendarwidget.h
    lunarcalendarwidget/picturetowhite.h
    lunarcalendarwidget/customstylePushbutton.h
)

set(SOURCES
    ukuicalendar.cpp
    ukuiwebviewdialog.cpp
    lunarcalendarwidget/frmlunarcalendarwidget.cpp
    lunarcalendarwidget/lunarcalendarinfo.cpp
    lunarcalendarwidget/lunarcalendaritem.cpp
    lunarcalendarwidget/lunarcalendarwidget.cpp
    lunarcalendarwidget/picturetowhite.cpp
    lunarcalendarwidget/customstylePushbutton.cpp
)

set(UIS
    ukuiwebviewdialog.ui
    lunarcalendarwidget/frmlunarcalendarwidget.ui
)

find_package(Qt5WebKit)
find_package(Qt5WebKitWidgets)
find_package(PkgConfig)
pkg_check_modules(Gsetting REQUIRED gsettings-qt)
pkg_check_modules(CALENDAR_DEPS REQUIRED glib-2.0)
include_directories(${CALENDAR_DEPS_INCLUDE_DIRS})
include_directories(${Gsetting_INCLUDE_DIRS})


ADD_DEFINITIONS(-DQT_NO_KEYWORDS)
link_libraries(libQt5WebKitWidgets.so.5 libQt5WebKit.so.5 glib-2.0.so)
install(DIRECTORY html/
        DESTINATION ${PACKAGE_DATA_DIR}/plugin-calendar/html)

install(DIRECTORY style/
        DESTINATION ${PACKAGE_DATA_DIR}/plugin-calendar/style)

BUILD_UKUI_PLUGIN(${PLUGIN})
