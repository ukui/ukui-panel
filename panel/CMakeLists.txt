set(PROJECT ukui-panel)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(common)
#add_subdirectory(xdg)
#add_subdirectory(common_fun)

set(PRIV_HEADERS
    panelpluginsmodel.h
    ukuipanel.h
    ukuipanelapplication.h
    ukuipanelapplication_p.h
    ukuipanellayout.h
    plugin.h
    pluginsettings_p.h
)

# using UKUi namespace in the public headers.
set(PUB_HEADERS
    ukuipanelglobals.h
    pluginsettings.h
    iukuipanelplugin.h
    iukuipanel.h

    common/ukuisettings.h
    common/ukuiplugininfo.h
    common/ukuitranslator.h
    common/common.h

    common/ukuigridlayout.h
    common/ukuiglobals.h
    common_fun/listengsettings.h
    common_fun/ukuipanel_infomation.h
    common_fun/dbus-adaptor.h
    common_fun/panel_commission.h
    customstyle.h
)

set(SOURCES
    main.cpp
    panelpluginsmodel.cpp
    windownotifier.cpp
    ukuipanel.cpp
    ukuipanelapplication.cpp
    ukuipanellayout.cpp
    plugin.cpp
    pluginsettings.cpp

    common/ukuiplugininfo.cpp
    common/ukuisettings.cpp
    common/ukuitranslator.cpp

    common/ukuigridlayout.cpp
    common_fun/listengsettings.cpp
    common_fun/ukuipanel_infomation.cpp
    common_fun/dbus-adaptor.cpp
    common_fun/panel_commission.cpp
    customstyle.cpp
)

file(GLOB CONFIG_FILES resources/*.conf resources/*.qss)
############################################

add_definitions(-DCOMPILE_UKUI_PANEL)

set(PLUGIN_DESKTOPS_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/ukui/${PROJECT}")
add_definitions(-DPLUGIN_DESKTOPS_DIR=\"${PLUGIN_DESKTOPS_DIR}\")

if (WITH_SCREENSAVER_FALLBACK)
    message(STATUS "Building with conversion of deprecated 'screensaver' plugin")
    add_definitions(-DWITH_SCREENSAVER_FALLBACK "-DUKUI_LOCK_DESKTOP=\"${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications/ukui-lockscreen.desktop\"")
endif ()

project(${PROJECT})

set(QTX_LIBRARIES Qt5::Widgets Qt5::Xml Qt5::DBus)

 #Translations
#ukui_translate_ts(QM_FILES SOURCES
#    UPDATE_TRANSLATIONS
#        ${UPDATE_TRANSLATIONS}
#    SOURCES
#        ${PUB_HEADERS}
#        ${PRIV_HEADERS}
#        ${SOURCES}
#        ${UI}
#    INSTALL_DIR
#    "${UKUI_TRANSLATIONS_DIR}/${PROJECT_NAME}"
#)

ukui_app_translation_loader(SOURCES ${PROJECT_NAME})

set_property(SOURCE ${DBUS_INTERFACE_SRCS} ${DBUS_ADAPTOR_SRCS} PROPERTY SKIP_AUTOGEN ON)
list(APPEND SOURCES "${DBUS_INTERFACE_SRCS}" "${DBUS_ADAPTOR_SRCS}")




add_executable(${PROJECT}
    ${PUB_HEADERS}
    ${PRIV_HEADERS}
    ${QM_FILES}
    ${SOURCES}
    ${UI}
)
find_package(PkgConfig)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)
pkg_check_modules(GIO2 REQUIRED gio-2.0)
pkg_check_modules(Gsetting REQUIRED gsettings-qt)
include_directories(${GLIB2_INCLUDE_DIRS})
include_directories(${GIO2_INCLUDE_DIRS})
include_directories(${Gsetting_INCLUDE_DIRS})
#ADD_DEFINITIONS(-DQT_NO_KEYWORDS)
target_link_libraries(${PROJECT}
    ${LIBRARIES}
    ${QTX_LIBRARIES}
    KF5::WindowSystem
    ${STATIC_PLUGINS}
    Qt5Xdg
    ${GLIB2_LIBRARIES}
    ${GIO2_LIBRARIES}
    ${Gsetting_LIBRARIES}
)


target_compile_definitions(${PROJECT}
    PRIVATE
        "UKUI_RELATIVE_SHARE_DIR=\"${UKUI_RELATIVE_SHARE_DIR}\""
        #"UKUI_SHARE_DIR=\"${UKUI_SHARE_DIR}\""
        "UKUI_RELATIVE_SHARE_TRANSLATIONS_DIR=\"${UKUI_RELATIVE_TRANSLATIONS_DIR}\""
        "UKUI_SHARE_TRANSLATIONS_DIR=\"${UKUI_TRANSLATIONS_DIR}\""
        "UKUI_GRAPHICS_DIR=\"${UKUI_GRAPHICS_DIR}\""
        #"UKUI_ETC_XDG_DIR=\"${UKUI_ETC_XDG_DIR}\""
        "UKUI_DATA_DIR=\"${UKUI_DATA_DIR}\""   
        "UKUI_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\""
        #"UKUI_VERSION=\"${UKUI_VERSION}\""
        #"COMPILE_LIBUKUI"
        #"QT_USE_QSTRINGBUILDER"
        #"QT_NO_CAST_FROM_ASCII"
        #"QT_NO_CAST_TO_ASCII"
        #"QT_NO_URL_CAST_FROM_STRING"
        #"QT_NO_CAST_FROM_BYTEARRAY"
        #"$<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>"
        #"$<$<CONFIG:Release>:QT_NO_WARNING_OUTPUT>"
)


install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES ${CONFIG_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/ukui)
install(FILES ${PUB_HEADERS} DESTINATION include/ukui)
install(FILES
    ../man/ukui-panel.1
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
    COMPONENT Runtime
)
install(FILES
    resources/ukui-panel.desktop
    DESTINATION "/etc/xdg/autostart/"
    COMPONENT Runtime
)

install(DIRECTORY
	./img/
    DESTINATION "${PACKAGE_DATA_DIR}/panel/img"
)

install(FILES
    resources/ukui-panel_zh_CN.qm resources/ukui-panel_zh_CN.ts resources/ukui-panel_tr.ts
    DESTINATION "${PACKAGE_DATA_DIR}/panel/resources"
    COMPONENT Runtime
)

install(FILES
    resources/org.ukui.panel.settings.gschema.xml
    DESTINATION "/usr/share/glib-2.0/schemas"
    COMPONENT Runtime
)

install(FILES
    resources/panel-commission.sh  resources/panel-commission.ini  resources/ukui-panel-reset.sh resources/ukui-panel-config.sh
    DESTINATION "/usr/share/ukui/ukui-panel"
    COMPONENT Runtime
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ GROUP_WRITE WORLD_READ WORLD_WRITE WORLD_EXECUTE
    GROUP_EXECUTE GROUP_READ
)

set(PLUGIN panel)
include(../cmake/UkuiPluginTranslationTs.cmake)
ukui_plugin_translate_ts(${PLUGIN})

