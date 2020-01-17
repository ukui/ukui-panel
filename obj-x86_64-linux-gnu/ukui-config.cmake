# - Finds the ukui package


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was ukui-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

if (CMAKE_VERSION VERSION_LESS 3.0.2)
    message(FATAL_ERROR \"ukui-panel requires at least CMake version 3.0.2\")
endif()

include(CMakeFindDependencyMacro)

find_dependency(Qt5Widgets 5.7.1)
find_dependency(Qt5DBus 5.7.1)
find_dependency(Qt5X11Extras 5.7.1)
find_dependency(Qt5LinguistTools 5.7.1)
find_dependency(Qt5Xdg 3.3.1)
find_dependency(KF5WindowSystem)
#find_dependency(ukui-build-tools 0.6.0)
#include(UKUiConfigVars)
include(/usr/share/cmake/ukui-build-tools/modules/UKUiConfigVars.cmake)


#  - Set version informations
set(UKUI_MAJOR_VERSION      "0")
set(UKUI_MINOR_VERSION      "14")
set(UKUI_PATCH_VERSION      "1")
set(UKUI_VERSION            "0.14.1")

add_definitions("-DUKUI_MAJOR_VERSION=\"${UKUI_MAJOR_VERSION}\"")
add_definitions("-DUKUI_MINOR_VERSION=\"${UKUI_MINOR_VERSION}\"")
add_definitions("-DUKUI_PATCH_VERSION=\"${UKUI_PATCH_VERSION}\"")
add_definitions("-DUKUI_VERSION=\"${UKUI_VERSION}\"")

if (NOT TARGET ukui)
    if (POLICY CMP0024)
        cmake_policy(SET CMP0024 NEW)
    endif()
    include("${CMAKE_CURRENT_LIST_DIR}/ukui-targets.cmake")
endif()
