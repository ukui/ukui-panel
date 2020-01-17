# Install script for directory: /home/kylin/work/panel/ukui-panel/panel

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/ukui-panel" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/ukui-panel")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/ukui-panel"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/kylin/work/panel/ukui-panel/obj-x86_64-linux-gnu/panel/ukui-panel")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/ukui-panel" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/ukui-panel")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/ukui-panel")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/ukui" TYPE FILE FILES
    "/home/kylin/work/panel/ukui-panel/panel/resources/panel.conf"
    "/home/kylin/work/panel/ukui-panel/panel/resources/panel.qss"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ukui" TYPE FILE FILES
    "/home/kylin/work/panel/ukui-panel/panel/ukuipanelglobals.h"
    "/home/kylin/work/panel/ukui-panel/panel/pluginsettings.h"
    "/home/kylin/work/panel/ukui-panel/panel/iukuipanelplugin.h"
    "/home/kylin/work/panel/ukui-panel/panel/iukuipanel.h"
    "/home/kylin/work/panel/ukui-panel/panel/comm_func.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuihtmldelegate.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuisettings.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuiplugininfo.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuiapplication.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuisingleapplication.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuitranslator.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuiprogramfinder.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/configdialog/ukuiconfigdialog.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/configdialog/ukuiconfigdialogcmdlineoptions.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/configdialog/ukuipageselectwidget.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuinotification.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuiautostartentry.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuigridlayout.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuirotatedwidget.h"
    "/home/kylin/work/panel/ukui-panel/panel/common/ukuiglobals.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xRuntimex" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man1" TYPE FILE FILES "/home/kylin/work/panel/ukui-panel/panel/man/ukui-panel.1")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xRuntimex" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/autostart/ukui-panel.desktop")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/autostart" TYPE FILE FILES "/home/kylin/work/panel/ukui-panel/panel/resources/ukui-panel.desktop")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/kylin/work/panel/ukui-panel/obj-x86_64-linux-gnu/panel/common/cmake_install.cmake")

endif()

