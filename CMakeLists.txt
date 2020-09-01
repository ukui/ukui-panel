cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(ukui-panel)

option(WITH_SCREENSAVER_FALLBACK "Include support for converting the deprecated 'screensaver' plugin to 'quicklaunch'. This requires the ukui-leave (ukui-session) to be installed in runtime." OFF)

#判断编译器类型,如果是gcc编译器,则在编译选项中加入c++11支持
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")   
endif(CMAKE_COMPILER_IS_GNUCXX)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

macro(setByDefault VAR_NAME VAR_VALUE)
  if(NOT DEFINED ${VAR_NAME})
    set (${VAR_NAME} ${VAR_VALUE})
  endif(NOT DEFINED ${VAR_NAME})
endmacro()

include(GNUInstallDirs)

setByDefault(CUSTOM_QT_5_6_VERSION Yes)
setByDefault(CUSTOM_QT_5_12_VERSION No)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(REQUIRED_QT_VERSION "5.6.1")
set(KF5_MINIMUM_VERSION "5.18.0")
set(QT_MINIMUM_VERSION "5.6.1")
set(UKUI_MINIMUM_VERSION "0.14.1")
set(QTXDG_MINIMUM_VERSION "3.3.1")

find_package(Qt5DBus ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5LinguistTools ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5X11Extras ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Xml ${REQUIRED_QT_VERSION} REQUIRED)
find_package(KF5WindowSystem ${KF5_MINIMUM_VERSION} REQUIRED)
find_package(Qt5 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Widgets DBus X11Extras LinguistTools)
find_package(Qt5Xdg ${QTXDG_MINIMUM_VERSION} REQUIRED)
find_package(X11 REQUIRED)
find_package(Qt5LinguistTools)

find_package(PkgConfig)
pkg_check_modules(Gsetting REQUIRED gsettings-qt)
include_directories(${Gsetting_INCLUDE_DIRS})
set(LIBRARIES
    ${Gsetting_LIBRARIES}
)

# Patch Version
set(UKUI_VERSION 3.0)
set(UKUI_PANEL_PATCH_VERSION 0)
set(UKUI_MAJOR_VERSION 3)
set(UKUI_MINOR_VERSION 0)

#set(UKUI_TRANSLATIONS_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/ukui/translations/")
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
include(./cmake/ukui-build-tools/modules/UKUiTranslateDesktop.cmake)
include(./cmake/ukui-build-tools/modules/UKUiTranslationLoader.cmake)
set(ENABLED_PLUGINS) # list of enabled plugins
set(STATIC_PLUGINS) # list of statically linked plugins

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

add_subdirectory(ukui-flash-disk)

setByDefault(TRAY_PLUGIN Yes)
if(TRAY_PLUGIN)
    list(APPEND STATIC_PLUGINS "tray")
    add_definitions(-DWITH_TRAY_PLUGIN)
    list(APPEND ENABLED_PLUGINS "System Tray")
    add_subdirectory(plugin-tray)
endif()

setByDefault(STATUSNOTIFIER_PLUGIN Yes)
if(STATUSNOTIFIER_PLUGIN)
    list(APPEND STATIC_PLUGINS "statusnotifier")
    add_definitions(-DWITH_STATUSNOTIFIER_PLUGIN)
    list(APPEND ENABLED_PLUGINS "Status Notifier")
    add_subdirectory(plugin-statusnotifier)
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
    list(APPEND ENABLED_PLUGINS "calendar")
    add_subdirectory(plugin-calendar)
endif(CALENDAR_PLUGIN)

setByDefault(STARTMENU_PLUGIN Yes)
if(STARTMENU_PLUGIN)
    list(APPEND ENABLED_PLUGINS "startmenu")
    add_subdirectory(plugin-startmenu)
endif(STARTMENU_PLUGIN)

setByDefault(NIGHTMODE_PLUGIN Yes)
if(NIGHTMODE_PLUGIN)
    list(APPEND ENABLED_PLUGINS "nightmode")
    add_subdirectory(plugin-nightmode)
endif(NIGHTMODE_PLUGIN)

setByDefault(TASKVIEW_PLUGIN Yes)
if(TASKVIEW_PLUGIN)
    list(APPEND ENABLED_PLUGINS "taskview")
    add_subdirectory(plugin-taskview)
endif(TASKVIEW_PLUGIN)

setByDefault(SEGMENTATION_PLUGIN Yes)
if(SEGMENTATION_PLUGIN)
    list(APPEND ENABLED_PLUGINS "segmentation")
    add_subdirectory(plugin-segmentation)
endif(SEGMENTATION_PLUGIN)

#########################################################################

message(STATUS  "**************** The following plugins will be built ****************")
foreach (PLUGIN_STR ${ENABLED_PLUGINS})
    message(STATUS "  ${PLUGIN_STR}")
endforeach()
message(STATUS "*********************************************************************")

add_subdirectory(panel)

file(GLOB_RECURSE QRC_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

# translation
set(UKUI_TRANSLATIONS_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/ukui/)
add_definitions(
    -DUKUI_TRANSLATIONS_DIR="${UKUI_TRANSLATIONS_DIR}"
)
if (NOT DEFINED UPDATE_TRANSLATIONS)
    set(UPDATE_TRANSLATIONS "No")
endif()

# To create a new ts file: lupdate -recursive . -target-language zh_CN -ts panel/resources/ukui-panel_zh_CN.ts
file(GLOB TS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/panel/resources/*.ts")

# cmake -DUPDATE_TRANSLATIONS=yes
if (UPDATE_TRANSLATIONS)
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_add_translation(QM_FILES ${TS_FILES})
endif()
add_custom_target(translations ALL DEPENDS ${QM_FILES})
install(FILES ${QM_FILES} DESTINATION ${UKUI_TRANSLATIONS_DIR})

