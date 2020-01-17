# Install script for directory: /home/minglequn/hepuyaolib/ukui-build-tools-0.6.0

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/ukui-build-tools" TYPE FILE FILES
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/obj-x86_64-linux-gnu/install/ukui-build-tools-config.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/obj-x86_64-linux-gnu/ukui-build-tools-config-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/ukui-build-tools/find-modules" TYPE FILE FILES
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/find-modules/FindExif.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/find-modules/FindGLIB.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/find-modules/FindMenuCache.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/find-modules/FindUDev.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/find-modules/FindXCB.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/find-modules/FindXKBCommon.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/find-modules/FindXdgUserDirs.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/ukui-build-tools/modules" TYPE FILE FILES
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/ECMFindModuleHelpers.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/Qt5PatchedLinguistToolsMacros.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/Qt5TranslationLoader.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/Qt5TranslationLoader.cpp.in"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiAppTranslationLoader.cpp.in"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiCompilerSettings.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiCreatePkgConfigFile.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiCreatePortableHeaders.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiLibTranslationLoader.cpp.in"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiPluginTranslationLoader.cpp.in"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiPreventInSourceBuilds.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiTranslate.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiTranslateDesktop.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiTranslateTs.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/cmake/modules/UKUiTranslationLoader.cmake"
    "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/obj-x86_64-linux-gnu/install/UKUiConfigVars.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/ukui-transupdate")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/minglequn/hepuyaolib/ukui-build-tools-0.6.0/obj-x86_64-linux-gnu/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
