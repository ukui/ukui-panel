
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
    lunarcalendarwidget/lunarcalendaryearitem.h
    lunarcalendarwidget/lunarcalendarmonthitem.h
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
    lunarcalendarwidget/lunarcalendaryearitem.cpp
    lunarcalendarwidget/lunarcalendarmonthitem.cpp
    lunarcalendarwidget/lunarcalendarwidget.cpp
    lunarcalendarwidget/picturetowhite.cpp
    lunarcalendarwidget/customstylePushbutton.cpp
)

set(UIS
    ukuiwebviewdialog.ui
    lunarcalendarwidget/frmlunarcalendarwidget.ui
)

find_package(PkgConfig)
pkg_check_modules(Gsetting REQUIRED gsettings-qt)
pkg_check_modules(CALENDAR_DEPS REQUIRED glib-2.0)
include_directories(${CALENDAR_DEPS_INCLUDE_DIRS})
include_directories(${Gsetting_INCLUDE_DIRS})


ADD_DEFINITIONS(-DQT_NO_KEYWORDS)
link_libraries(glib-2.0.so)
include(../cmake/UkuiPluginTranslationTs.cmake)
ukui_plugin_translate_ts(${PLUGIN})

install(DIRECTORY html/
        DESTINATION ${PACKAGE_DATA_DIR}/plugin-calendar/html)

BUILD_UKUI_PLUGIN(${PLUGIN})
