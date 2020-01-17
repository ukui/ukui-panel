# Install script for directory: /home/kylin/work/panel/ukui-panel/plugin-startmenu

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xRuntimex" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/share/ukui-panel/plugin-startmenu/img/startmenu.svg;/usr/share/ukui-panel/plugin-startmenu/img/taskview.svg;/usr/share/ukui-panel/plugin-startmenu/img/ukui-panel_zh_CN.qm")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/share/ukui-panel/plugin-startmenu/img" TYPE FILE FILES
    "/home/kylin/work/panel/ukui-panel/plugin-startmenu/img/startmenu.svg"
    "/home/kylin/work/panel/ukui-panel/plugin-startmenu/img/taskview.svg"
    "/home/kylin/work/panel/ukui-panel/plugin-startmenu/img/ukui-panel_zh_CN.qm"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/lib/x86_64-linux-gnu/ukui-panel/libstartmenu.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/lib/x86_64-linux-gnu/ukui-panel/libstartmenu.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/lib/x86_64-linux-gnu/ukui-panel/libstartmenu.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/lib/x86_64-linux-gnu/ukui-panel/libstartmenu.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/lib/x86_64-linux-gnu/ukui-panel" TYPE MODULE FILES "/home/kylin/work/panel/ukui-panel/obj-x86_64-linux-gnu/plugin-startmenu/libstartmenu.so")
  if(EXISTS "$ENV{DESTDIR}/usr/lib/x86_64-linux-gnu/ukui-panel/libstartmenu.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/lib/x86_64-linux-gnu/ukui-panel/libstartmenu.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/lib/x86_64-linux-gnu/ukui-panel/libstartmenu.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/share/ukui/ukui-panel/startmenu.desktop")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/share/ukui/ukui-panel" TYPE FILE FILES "/home/kylin/work/panel/ukui-panel/obj-x86_64-linux-gnu/plugin-startmenu/startmenu.desktop")
endif()

