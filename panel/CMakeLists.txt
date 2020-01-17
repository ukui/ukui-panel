set(PROJECT ukui-panel)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(common)

set(PRIV_HEADERS
    panelpluginsmodel.h
    windownotifier.h
    ukuipanel.h
    ukuipanelapplication.h
    ukuipanelapplication_p.h
    ukuipanellayout.h
    plugin.h
    pluginsettings_p.h
    ukuipanellimits.h
    popupmenu.h
    pluginmoveprocessor.h
    ukuipanelpluginconfigdialog.h
    config/configpaneldialog.h
    config/configpanelwidget.h
    config/configpluginswidget.h
    config/addplugindialog.h
)

# using UKUi namespace in the public headers.
set(PUB_HEADERS
    ukuipanelglobals.h
    pluginsettings.h
    iukuipanelplugin.h
    iukuipanel.h
    comm_func.h

    common/ukuihtmldelegate.h
    common/ukuisettings.h
    common/ukuiplugininfo.h
    #common/ukuipowermanager.h
    #common/ukuiscreensaver.h
    common/ukuiapplication.h
    common/ukuisingleapplication.h
    common/ukuitranslator.h
    common/ukuiprogramfinder.h

    common/configdialog/ukuiconfigdialog.h
    common/configdialog/ukuiconfigdialogcmdlineoptions.h
    common/configdialog/ukuipageselectwidget.h

    #common/ukuipower/ukuipower.h
    common/ukuinotification.h
    common/ukuiautostartentry.h
    common/ukuigridlayout.h
    common/ukuirotatedwidget.h
    common/ukuiglobals.h
    #common/ukuibacklight.h
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
    popupmenu.cpp
    pluginmoveprocessor.cpp
    ukuipanelpluginconfigdialog.cpp
    config/configpaneldialog.cpp
    config/configpanelwidget.cpp
    config/configpluginswidget.cpp
    config/addplugindialog.cpp
    comm_func.cpp

    common/ukuihtmldelegate.cpp
    common/ukuiplugininfo.cpp
    #common/ukuipowermanager.cpp
    common/ukuisettings.cpp
    #common/ukuiscreensaver.cpp
    common/ukuiapplication.cpp
    common/ukuisingleapplication.cpp
    common/ukuitranslator.cpp
    common/ukuiprogramfinder.cpp

    common/configdialog/ukuiconfigdialog.cpp
    common/configdialog/ukuiconfigdialogcmdlineoptions.cpp
    common/configdialog/ukuipageselectwidget.cpp

    #common/ukuipower/ukuipower.cpp    
    #common/ukuipower/ukuipowerproviders.cpp
    common/ukuiautostartentry.cpp
    common/ukuinotification.cpp
    common/ukuigridlayout.cpp
    common/ukuirotatedwidget.cpp
    #common/ukuibacklight.cpp
    #common/ukuibacklight/virtual_backend.cpp
    #common/ukuibacklight/linux_backend/linuxbackend.cpp
)

set(UI
    config/configpanelwidget.ui
    config/configpluginswidget.ui
    config/addplugindialog.ui
    common/configdialog/ukuiconfigdialog.ui
)

#set(LIBRARIES
#    ukui
#)

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


QT5_ADD_DBUS_INTERFACE(DBUS_INTERFACE_SRCS
    common/dbus/org.freedesktop.Notifications.xml
    notifications_interface
)

QT5_ADD_DBUS_ADAPTOR(DBUS_ADAPTOR_SRCS
    common/dbus/org.ukui.SingleApplication.xml
    common/ukuisingleapplication.h UKUi::SingleApplication
)

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
    man/ukui-panel.1
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
    COMPONENT Runtime
)
install(FILES
    resources/ukui-panel.desktop
    DESTINATION "${UKUI_ETC_XDG_DIR}/autostart/"
    COMPONENT Runtime
)
