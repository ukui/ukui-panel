set(PLUGIN "tray")

include(CheckLibraryExists)

#find_package(XCB REQUIRED COMPONENTS xcb xcb-util xcb-damage)
find_package(PkgConfig)

find_package(X11 REQUIRED)
find_package(Qt5X11Extras ${REQUIRED_QT_VERSION} REQUIRED)
pkg_check_modules(XCOMPOSITE REQUIRED xcomposite)
pkg_check_modules(XDAMAGE REQUIRED xdamage)
pkg_check_modules(XRENDER REQUIRED xrender)
pkg_check_modules(Gsetting REQUIRED gsettings-qt)
pkg_check_modules(GIO2 REQUIRED gio-2.0)
pkg_check_modules(GIOUNIX2 REQUIRED gio-unix-2.0)
pkg_check_modules(DCONF REQUIRED dconf)


include_directories(${Gsetting_INCLUDE_DIRS})
include_directories(${GIO2_INCLUDE_DIRS})
include_directories(${GIOUNIX2_INCLUDE_DIRS})
include_directories(${DCONF_INCLUDE_DIRS})

set(HEADERS
    ukuitrayplugin.h
    ukuitray.h
    trayicon.h
    xfitman.h
    traystorage.h
    highlight-effect.h
)

set(SOURCES
    ukuitrayplugin.cpp
    ukuitray.cpp
    trayicon.cpp
    xfitman.cpp
    traystorage.cpp
    highlight-effect.cpp
)

set(LIBRARIES
    ${X11_LIBRARIES}
    ${XCOMPOSITE_LDFLAGS}
    ${XDAMAGE_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XCB_LIBRARIES}
    Qt5X11Extras
    ${Gsetting_LIBRARIES}
    ${GIO2_LIBRARIES}
    ${GIOUNIX2_LIBRARIES}
    ${DCONF_LIBRARIES}
)

install(FILES
    ../panel/resources/org.ukui.panel.tray.gschema.xml
    DESTINATION "/usr/share/glib-2.0/schemas/"
    COMPONENT Runtime
)
BUILD_UKUI_PLUGIN(${PLUGIN})
