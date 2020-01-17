cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
# CMP0000: Call the cmake_minimum_required() command at the beginning of the top-level
# CMakeLists.txt file even before calling the project() command.
# The cmake_minimum_required(VERSION) command implicitly invokes the cmake_policy(VERSION)
# command to specify that the current project code is written for the given range of CMake
# versions.
project(ukui-panel)


option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)
option(WITH_SCREENSAVER_FALLBACK "Include support for converting the deprecated 'screensaver' plugin to 'quicklaunch'. This requires the ukui-leave (ukui-session) to be installed in runtime." OFF)
# plugin-mainmenu
option(USE_MENU_CACHE "Use menu-cached (no noticable penalty even on a 2004 single core pentium if not used)" OFF)



# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

macro(setByDefault VAR_NAME VAR_VALUE)
  if(NOT DEFINED ${VAR_NAME})
    set (${VAR_NAME} ${VAR_VALUE})
  endif(NOT DEFINED ${VAR_NAME})
endmacro()

include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(REQUIRED_QT_VERSION "5.7.1")
set(KF5_MINIMUM_VERSION "5.36.0")
set(UKUI_MINIMUM_VERSION "0.14.1")
set(QTXDG_MINIMUM_VERSION "3.3.1")
set(QT_MINIMUM_VERSION "5.7.1")

find_package(Qt5DBus ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5LinguistTools ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5X11Extras ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Xml ${REQUIRED_QT_VERSION} REQUIRED)
find_package(KF5WindowSystem ${KF5_MINIMUM_VERSION} REQUIRED)
#find_package(ukui ${UKUI_MINIMUM_VERSION} REQUIRED)
#find_package(ukui-globalkeys ${UKUI_MINIMUM_VERSION} REQUIRED)
#find_package(ukui-globalkeys-ui ${UKUI_MINIMUM_VERSION} REQUIRED)
find_package(Qt5 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Widgets DBus X11Extras LinguistTools)
find_package(Qt5Xdg ${QTXDG_MINIMUM_VERSION} REQUIRED)
find_package(X11 REQUIRED)

# Patch Version
set(UKUI_VERSION 3.0)
set(UKUI_PANEL_PATCH_VERSION 0)
set(UKUI_MAJOR_VERSION 3)
set(UKUI_MINOR_VERSION 0)

set(UKUI_TRANSLATIONS_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/ukui/translations/")
set(UKUI_PANEL_VERSION ${UKUI_MAJOR_VERSION}.${UKUI_MINOR_VERSION}.${UKUI_PANEL_PATCH_VERSION})
add_definitions("-DUKUI_PANEL_VERSION=\"${UKUI_PANEL_VERSION}\"")

include(./cmake/ukui-build-tools/modules/UKUiPreventInSourceBuilds.cmake)
#include(./cmake/ukui-build-tools/modules/UKUiTranslate.cmake)

# All UKUiCompilerSettings except CMAKE_MODULE_LINKER_FLAGS work just fine
# So we reset only these Flags after loading UKUiCompilerSettings
# ukui-build-tools:
#  set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-undefined ${SYMBOLIC_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}")
message(STATUS "==OLD== CMAKE_MODULE_LINKER_FLAGS: ${CMAKE_MODULE_LINKER_FLAGS}")
set( OLD_CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
#include(UKUiCompilerSettings NO_POLICY_SCOPE)
set(CMAKE_MODULE_LINKER_FLAGS "${OLD_CMAKE_MODULE_LINKER_FLAGS} ${SYMBOLIC_FLAGS}")

# Warning: This must be before add_subdirectory(panel). Move with caution.
set(PLUGIN_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/ukui-panel")
add_definitions(
    -DPLUGIN_DIR=\"${PLUGIN_DIR}\"
)
#Add PACKAGE_DATA_DIR
set(PACKAGE_DATA_DIR "/usr/share/ukui-panel")
add_definitions(
    -DPACKAGE_DATA_DIR=\"${PACKAGE_DATA_DIR}\"
)

message(STATUS "CMAKE Module linker flags: ${CMAKE_MODULE_LINKER_FLAGS}")
message(STATUS "Panel plugins location: ${PLUGIN_DIR}")

#########################################################################
# Plugin system
# You can enable/disable building of the plugin using cmake options.
#    cmake -DWORLDCLOCK_PLUGIN=Yes .. # Enable worldclock plugin
#    cmake -DWORLDCLOCK_PLUGIN=No ..  # Disable worldclock plugin

include("cmake/BuildPlugin.cmake")
#include(./cmake/ukui-build-tools/modules/UKUiTranslateDesktop.cmake)
include(./cmake/ukui-build-tools/modules/UKUiTranslateDesktop.cmake)
#include(./cmake/ukui-build-tools/modules/UKUiTranslationLoader.cmake)
include(./cmake/ukui-build-tools/modules/UKUiTranslationLoader.cmake)
set(ENABLED_PLUGINS) # list of enabled plugins
set(STATIC_PLUGINS) # list of statically linked plugins


#setByDefault(DESKTOPSWITCH_PLUGIN Yes)
if(DESKTOPSWITCH_PLUGIN)
    list(APPEND STATIC_PLUGINS "desktopswitch")
    add_definitions(-DWITH_DESKTOPSWITCH_PLUGIN)
    list(APPEND ENABLED_PLUGINS "Desktop Switcher")
    add_subdirectory(plugin-desktopswitch)
endif()


#setByDefault(MAINMENU_PLUGIN Yes)
if(MAINMENU_PLUGIN)
    list(APPEND STATIC_PLUGINS "mainmenu")
    add_definitions(-DWITH_MAINMENU_PLUGIN)
    list(APPEND ENABLED_PLUGINS "Application menu")
    add_subdirectory(plugin-mainmenu)
endif()

#setByDefault(MOUNT_PLUGIN Yes)
if(MOUNT_PLUGIN)
    list(APPEND ENABLED_PLUGINS "Mount")
    add_subdirectory(plugin-mount)
endif(MOUNT_PLUGIN)

setByDefault(QUICKLAUNCH_PLUGIN Yes)
if(QUICKLAUNCH_PLUGIN)
    list(APPEND STATIC_PLUGINS "quicklaunch")
    add_definitions(-DWITH_QUICKLAUNCH_PLUGIN)
    list(APPEND ENABLED_PLUGINS "Quicklaunch")
    add_subdirectory(plugin-quicklaunch)
endif()


setByDefault(SHOWDESKTOP_PLUGIN Yes)
if(SHOWDESKTOP_PLUGIN)
    list(APPEND STATIC_PLUGINS "showdesktop")
    add_definitions(-DWITH_SHOWDESKTOP_PLUGIN)
    list(APPEND ENABLED_PLUGINS "Show Desktop")
    add_subdirectory(plugin-showdesktop)
endif()


setByDefault(TASKBAR_PLUGIN Yes)
if(TASKBAR_PLUGIN)
  list(APPEND STATIC_PLUGINS "taskbar")
  add_definitions(-DWITH_TASKBAR_PLUGIN)
  list(APPEND ENABLED_PLUGINS "Taskbar")
  add_subdirectory(plugin-taskbar)
endif()


setByDefault(TRAY_PLUGIN Yes)
if(TRAY_PLUGIN)
    list(APPEND STATIC_PLUGINS "tray")
    add_definitions(-DWITH_TRAY_PLUGIN)
    list(APPEND ENABLED_PLUGINS "System Tray")
    add_subdirectory(plugin-tray)
endif()

#setByDefault(VOLUME_PLUGIN Yes)
setByDefault(VOLUME_USE_PULSEAUDIO Yes)
setByDefault(VOLUME_USE_ALSA Yes)
if(VOLUME_PLUGIN)
    if (VOLUME_USE_PULSEAUDIO)
        find_package(PulseAudio)
        if (NOT PULSEAUDIO_FOUND)
            message(FATAL_ERROR "PulseAudio not found, but required (VOLUME_USE_PULSEAUDIO) for Volume plugin!")
        endif ()
    endif(VOLUME_USE_PULSEAUDIO)

    if(VOLUME_USE_ALSA)
        find_package(ALSA)
        if (NOT ALSA_FOUND)
            message(FATAL_ERROR "ALSA not found, but required (VOLUME_USE_ALSA) for Volume plugin!")
        endif ()
    endif()

    list(APPEND ENABLED_PLUGINS   "Volume")
    message(STATUS "")
    message(STATUS "Volume plugin will be built")
    message(STATUS "    ALSA: ${ALSA_FOUND}")
    message(STATUS "    PulseAudio: ${PULSEAUDIO_FOUND}")
    message(STATUS "")
    add_subdirectory(plugin-volume)
endif()


setByDefault(SPACER_PLUGIN Yes)
if(SPACER_PLUGIN)
    list(APPEND STATIC_PLUGINS "spacer")
    add_definitions(-DWITH_SPACER_PLUGIN)
    list(APPEND ENABLED_PLUGINS "Spacer")
    add_subdirectory(plugin-spacer)
endif()

setByDefault(CALENDAR_PLUGIN Yes)
if(CALENDAR_PLUGIN)
#   list(APPEND STATIC_PLUGINS "calendar")
#    add_definitions(-DWITH_CALENDAR_PLUGIN)
    list(APPEND ENABLED_PLUGINS "calendar")
    add_subdirectory(plugin-calendar)
endif(CALENDAR_PLUGIN)

setByDefault(STARTMENU_PLUGIN Yes)
if(STARTMENU_PLUGIN)
#    list(APPEND STATIC_PLUGINS "startmenu")
#    add_definitions(-DWITH_STARTMENU_PLUGIN)
    list(APPEND ENABLED_PLUGINS "startmenu")
    add_subdirectory(plugin-startmenu)
endif(STARTMENU_PLUGIN)

setByDefault(POWERSWITCH_PLUGIN Yes)
if(POWERSWITCH_PLUGIN)
#    list(APPEND STATIC_PLUGINS "powerswitch")
#    add_definitions(-DWITH_POWERSWITCH_PLUGIN)
    list(APPEND ENABLED_PLUGINS "powerswitch")
    add_subdirectory(plugin-powerswitch)
endif(POWERSWITCH_PLUGIN)

setByDefault(TASKVIEW_PLUGIN Yes)
if(TASKVIEW_PLUGIN)
#    list(APPEND STATIC_PLUGINS "taskview")
#    add_definitions(-DWITH_TASKVIEW_PLUGIN)
    list(APPEND ENABLED_PLUGINS "taskview")
    add_subdirectory(plugin-taskview)
endif(TASKVIEW_PLUGIN)

setByDefault(SEGMENTATION_PLUGIN Yes)
if(SEGMENTATION_PLUGIN)
#    list(APPEND STATIC_PLUGINS "segmentation")
#    add_definitions(-DWITH_SEGMENTATION_PLUGIN)
    list(APPEND ENABLED_PLUGINS "segmentation")
    add_subdirectory(plugin-segmentation)
endif(SEGMENTATION_PLUGIN)

#setByDefault(PART_PLUGIN Yes)
if(PART_PLUGIN)
#    list(APPEND STATIC_PLUGINS "powerswitch")
#    add_definitions(-DWITH_POWERSWITCH_PLUGIN)
    list(APPEND ENABLED_PLUGINS "part")
    add_subdirectory(plugin-part)
endif(PART_PLUGIN)
#########################################################################

message(STATUS  "**************** The following plugins will be built ****************")
foreach (PLUGIN_STR ${ENABLED_PLUGINS})
    message(STATUS "  ${PLUGIN_STR}")
endforeach()
message(STATUS "*********************************************************************")

add_subdirectory(panel)

# merged from ukui-common
#add_subdirectory(autostart)
#add_subdirectory(menu)
file(GLOB_RECURSE QRC_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)


