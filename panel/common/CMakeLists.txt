
set(UKUIBT_MINIMUM_VERSION "0.6.0")
set(KF5_MINIMUM_VERSION "5.36.0")
set(QT_MINIMUM_VERSION "5.7.1")
set(QTXDG_MINIMUM_VERSION "3.3.1")

# Major UKUi Version, belong to all components
set(UKUI_MAJOR_VERSION 0)
# Minor UKUi Version, belong to all components
set(UKUI_MINOR_VERSION 14)
#
# Patch Version, belong *only* to the component
# UKUi is 0.13 - libukui is at patch version 0
# The official UKUi version will follow libukui.
#
# In a perfect world all components would have the same major- and minor- and
# patch-version as libukui - in real life it will be fine if every component
# has it's own patch version within a major/minor life cyle.
#
set(UKUI_PATCH_VERSION 1)
set(UKUI_VERSION ${UKUI_MAJOR_VERSION}.${UKUI_MINOR_VERSION}.${UKUI_PATCH_VERSION})


option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)


#find_package(ukui-build-tools ${UKUIBT_MINIMUM_VERSION} REQUIRED)
#find_package(Qt5 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Widgets DBus X11Extras LinguistTools)
#find_package(Qt5Xdg ${QTXDG_MINIMUM_VERSION} REQUIRED)
#find_package(KF5WindowSystem ${KF5_MINIMUM_VERSION} REQUIRED)
#find_package(PolkitQt5-1 REQUIRED)
#find_package(X11 REQUIRED)
message(STATUS "Building ${PROJECT_NAME} with Qt ${Qt5Core_VERSION}")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs) # Standard directories for installation


set(UKUI_PKG_CONFIG_DESCRIPTION "Shared library for UKUi applications")

set(HEADERS
    ukuisettings.h
    ukuiplugininfo.h
    ukuitranslator.h

    ukuigridlayout.h
    ukuiglobals.h
)

set(PUBLIC_CLASSES
    Settings
    PluginInfo
    Application
    SingleApplication
    Translator

    PageSelectWidget

    GridLayout
    Globals
)

set(SOURCES
    ukuiplugininfo.cpp
    ukuisettings.cpp
    ukuitranslator.cpp
    ukuiprogramfinder.cpp

    ukuigridlayout.cpp
)

set(MOCS
    ukuisettings.h
    ukuiscreensaver.h
    ukuiapplication.h

    ukuigridlayout.h
)

set(FORMS
    configdialog/ukuiconfigdialog.ui
)

file(GLOB UKUI_CONFIG_FILES resources/*.conf)

set_property(SOURCE ${DBUS_INTERFACE_SRCS} ${DBUS_ADAPTOR_SRCS} PROPERTY SKIP_AUTOGEN ON)
list(APPEND SOURCES "${DBUS_INTERFACE_SRCS}" "${DBUS_ADAPTOR_SRCS}")

# KF5WindowSystem is missing here. KF5WindowSystem doesn't provide an .pc file.
set(UKUI_PKG_CONFIG_REQUIRES "Qt5Xdg >= ${QTXDG_MINIMUM_VERSION}, Qt5Widgets >= ${QT_MINIMUM_VERSION}, Qt5Xml >= ${QT_MINIMUM_VERSION}, Qt5DBus >= ${QT_MINIMUM_VERSION}, Qt5X11Extras >= ${QT_MINIMUM_VERSION}")


# Standard directories for installation
include(../../cmake/ukui-build-tools/modules/UKUiPreventInSourceBuilds.cmake)
include(../../cmake/ukui-build-tools/modules/UKUiCompilerSettings.cmake)
include(../../cmake/ukui-build-tools/modules/UKUiCreatePkgConfigFile.cmake)
include(../../cmake/ukui-build-tools/modules/UKUiCreatePortableHeaders.cmake)
include(../../cmake/ukui-build-tools/modules/UKUiConfigVars.cmake)

set(UKUI_INTREE_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/include")

set(UKUI_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}/${UKUI_LIBRARY_NAME}")
set(UKUI_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/cmake")


## Translations

#include(../cmake/ukui-build-tools/modules/UKUiTranslateTs.cmake)

#ukui_translate_ts(QM_FILES
#   UPDATE_TRANSLATIONS
#       ${UPDATE_TRANSLATIONS}
#   SOURCES
#       ${SRCS}
#        ${FORMS}
#    INSTALL_DIR
#        "${UKUI_TRANSLATIONS_DIR}/${PROJECT_NAME}"
#)

message(STATUS "")
message(STATUS "libukui version: ${UKUI_VERSION}")
message(STATUS "")


# Copy public headers
foreach(h ${HEADERS})
    get_filename_component(bh ${h} NAME)
    configure_file(${h} "${UKUI_INTREE_INCLUDE_DIR}/UKUi/${bh}" COPYONLY)
endforeach()

# Create the portable headers
ukui_create_portable_headers(INTREE_PORTABLE_HEADERS
    NAME_PREFIX "ukui"
    OUTPUT_DIR "${UKUI_INTREE_INCLUDE_DIR}/UKUi"
    HEADER_NAMES ${PUBLIC_CLASSES}
)

#check_portable_headers(H_FILES ${PUB_HDRS} LINKS "${INTREE_PORTABLE_HEADERS}")

#************************************************
# Create in-tree build infrastructure
#************************************************
set(CFG_UKUI_TARGETS_FILE   "${UKUI_INTREE_TARGETS_FILE}")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ukui-config.cmake.in"
    "${CMAKE_BINARY_DIR}/${UKUI_LIBRARY_NAME}-config.cmake"
    INSTALL_DESTINATION "neverland"     # required, altough we don't install it
)

#************************************************
# Create installable build infrastructure
#************************************************
set(CFG_UKUI_TARGETS_FILE "${UKUI_INSTALL_CMAKE_DIR}/${UKUI_LIBRARY_NAME}/${UKUI_LIBRARY_NAME}-targets.cmake")

#install(EXPORT
#    ${UKUI_LIBRARY_NAME}-targets
#    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${UKUI_LIBRARY_NAME}"
#    COMPONENT Devel
#)


if (Qt5Core_VERSION VERSION_LESS "5.9.0")
    if (NOT DEFINED WITH_XDG_DIRS_FALLBACK)
        set(WITH_XDG_DIRS_FALLBACK ON)
    endif ()
elseif (WITH_XDG_DIRS_FALLBACK)
    set(WITH_XDG_DIRS_FALLBACK OFF)
    message(WARNING "Disabling requested WITH_XDG_DIRS_FALLBACK workaround, as proper implementation is in Qt from v5.9.0")
endif ()
if (WITH_XDG_DIRS_FALLBACK)
    message(STATUS "Building with homemade QSettings XDG fallback workaround")

setByDefault(CUSTOM_QT_5_12_VERSION Yes)
if (CUSTOM_QT_5_12_VERSION )
	#target_compile_definitions(${UKUI_LIBRARY_NAME}
	#PRIVATE "WITH_XDG_DIRS_FALLBACK"
	#)
endif(CUSTOM_QT_5_12_VERSION)
endif ()

#install(FILES
   # ${PUB_HDRS}
  #  DESTINATION "${UKUI_INSTALL_INCLUDE_DIR}/UKUi"
 #   COMPONENT Devel
#)

#install(FILES
   # ${INTREE_PORTABLE_HEADERS}
  #  DESTINATION "${UKUI_INSTALL_INCLUDE_DIR}/UKUi"
 #   COMPONENT Devel
#)

#install(FILES ${UKUI_CONFIG_FILES}
  #  DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/ukui"
 #   COMPONENT Runtime
#)

#install(FILES ${POLKIT_FILES} DESTINATION "${POLKITQT-1_POLICY_FILES_INSTALL_DIR}")

#************************************************
# Create and install pkgconfig file
#************************************************
ukui_create_pkgconfig_file(
    PACKAGE_NAME ${UKUI_LIBRARY_NAME}
    DESCRIPTIVE_NAME ${UKUI_LIBRARY_NAME}
    DESCRIPTION ${UKUI_PKG_CONFIG_DESCRIPTION}
    INCLUDEDIRS ${UKUI_LIBRARY_NAME}
    REQUIRES ${UKUI_PKG_CONFIG_REQUIRES}
    VERSION ${UKUI_VERSION}
    INSTALL
)
#************************************************
