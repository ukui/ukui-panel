set(PLUGIN "taskbar")

set(HEADERS
    ukuitaskbar.h
    ukuitaskbutton.h
    ukuitaskbarplugin.h
    ukuitaskgroup.h
    ukuigrouppopup.h
    ukuitaskwidget.h
    ukuitaskclosebutton.h
        quicklaunchaction.h
)

set(SOURCES
    ukuitaskbar.cpp
    ukuitaskbutton.cpp
    ukuitaskbarplugin.cpp
    ukuitaskgroup.cpp
    ukuigrouppopup.cpp
    ukuitaskwidget.cpp
    ukuitaskclosebutton.cpp
    quicklaunchaction.cpp
)

find_package(X11 REQUIRED)

find_package(PkgConfig)
pkg_check_modules(GIOUNIX2 REQUIRED gio-unix-2.0)
pkg_check_modules(GLIB2 REQUIRED glib-2.0 gio-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})
#for <QDBusInterface>
include_directories(${_Qt5DBus_OWN_INCLUDE_DIRS})
include_directories(
    ${UKUI_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/../panel"
    ${GIOUNIX2_INCLUDE_DIRS}
)

set(LIBRARIES
    Qt5Xdg
    ${GIOUNIX2_LIBRARIES}
    ${X11_LIBRARIES}
    Qt5X11Extras
    -lukui-com4cxx
)

install(FILES
    resources/name-icon.match
    DESTINATION "/usr/share/ukui/ukui-panel/plugin-taskbar"
    COMPONENT Runtime
)

include(../cmake/UkuiPluginTranslationTs.cmake)
ukui_plugin_translate_ts(${PLUGIN})

BUILD_UKUI_PLUGIN(${PLUGIN})

SET(_code "#include <KStartupInfo>
    #include <KWindowSystem>
    int main()
    {
        KStartupInfoData asn_data;
        asn_data.iconGeometry();
        return 0;
    }")
SET(_file ${CMAKE_CURRENT_BINARY_DIR}/quicklaunchaction.cpp)
FILE(WRITE "${_file}" "${_code}")
TRY_COMPILE(HAVE_STARTUP_ICON_GEOMETRY
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${_file}"
    LINK_LIBRARIES KF5::WindowSystem
    OUTPUT_VARIABLE OUTPUT)
MESSAGE("Build output:${OUTPUT} HAVE_STARTUP_ICON_GEOMETRY: ${HAVE_STARTUP_ICON_GEOMETRY}")
if(HAVE_STARTUP_ICON_GEOMETRY)
    add_definitions(-DHAVE_STARTUP_ICON_GEOMETRY=1)
endif()
