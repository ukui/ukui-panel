cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
# CMP0000: Call the cmake_minimum_required() command at the beginning of the top-level
# CMakeLists.txt file even before calling the project() command.
# The cmake_minimum_required(VERSION) command implicitly invokes the cmake_policy(VERSION)
# command to specify that the current project code is written for the given range of CMake
# versions.
project(ukui-build-tools)

option(WITH_XDG_DIRS_FALLBACK "Use our XDG_CONFIG_DIRS fallback" ON)

set(UKUI_BUILD_TOOLS_MAJOR_VERSION 0)
set(UKUI_BUILD_TOOLS_MINOR_VERSION 6)
set(UKUI_BUILD_TOOLS_PATCH_VERSION 0)
set(UKUI_BUILD_TOOLS_VERSION ${UKUI_BUILD_TOOLS_MAJOR_VERSION}.${UKUI_BUILD_TOOLS_MINOR_VERSION}.${UKUI_BUILD_TOOLS_PATCH_VERSION})


# Check for needed versions
# We need at least Qt 5.7.1 and glib-2.0 >= 2.50 to build all UKUi parts
find_package(PkgConfig REQUIRED)
find_package(Qt5Core "5.7.1"  REQUIRED)
pkg_check_modules(GLIB2 glib-2.0>=2.50 REQUIRED)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs) # Standard directories for installation
include(cmake/FindInstallConfigPath.cmake) # sets UKUI_ETC_XDG_DIR, if unset
include(cmake/modules/UKUiPreventInSourceBuilds.cmake)

#-----------------------------------------------------------------------------
# UKUI_DATA_DIR defaults to CMAKE_INSTALL_FULL_DATADIR. It's added to
#   XDG_DATA_DIRS by the startukui script
# Warning: Setting UKUI_DATA_DIR must be done after including GNUInstallDirs
#-----------------------------------------------------------------------------
if (NOT DEFINED UKUI_DATA_DIR)
    set(UKUI_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}" CACHE PATH
        "UKUi base directory relative to which data files should be searched"
    )
endif()

set(UKUI_LIBRARY_NAME "ukui" CACHE STRING "ukui")
set(UKUI_RELATIVE_SHARE_DIR "${UKUI_LIBRARY_NAME}")
set(UKUI_SHARE_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/${UKUI_RELATIVE_SHARE_DIR}")
set(UKUI_RELATIVE_TRANSLATIONS_DIR "${UKUI_LIBRARY_NAME}/translations")
set(UKUI_TRANSLATIONS_DIR "${UKUI_SHARE_DIR}/translations")
set(UKUI_GRAPHICS_DIR "${UKUI_SHARE_DIR}/graphics")

file(GLOB toInstallFindModules
    "${PROJECT_SOURCE_DIR}/cmake/find-modules/*.cmake"
)

file(GLOB toInstallModules
    "${PROJECT_SOURCE_DIR}/cmake/modules/*.cmake"
    "${PROJECT_SOURCE_DIR}/cmake/modules/*.in"
)


#-----------------------------------------------------------------------------
# Superbuild mode
#-----------------------------------------------------------------------------
set(FIND_MODULES_INSTALL_DIR "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ukui-build-tools/cmake/find-modules/")
set(MODULES_INSTALL_DIR "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ukui-build-tools/cmake/modules/")

file(COPY
    ${toInstallFindModules}
    DESTINATION "${FIND_MODULES_INSTALL_DIR}"
)

file(COPY
    ${toInstallModules}
    DESTINATION "${MODULES_INSTALL_DIR}"
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/ukui-build-tools-config.cmake.in"
    "${CMAKE_BINARY_DIR}/ukui-build-tools-config.cmake"
    INSTALL_DESTINATION "neverland"     # required, altough we don't install it
    PATH_VARS
        MODULES_INSTALL_DIR
        FIND_MODULES_INSTALL_DIR
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/UKUiConfigVars.cmake.in"
    "${MODULES_INSTALL_DIR}/UKUiConfigVars.cmake"
    @ONLY
)


#-----------------------------------------------------------------------------
# Installable mode
#-----------------------------------------------------------------------------
set(CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/cmake/ukui-build-tools/")
set(FIND_MODULES_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/cmake/ukui-build-tools/find-modules/")
set(MODULES_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/cmake/ukui-build-tools/modules/")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/ukui-build-tools-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/install/ukui-build-tools-config.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    PATH_VARS
        MODULES_INSTALL_DIR
        FIND_MODULES_INSTALL_DIR
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/UKUiConfigVars.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/install/UKUiConfigVars.cmake"
    @ONLY
)

# The package version file is common to superbuild and installable mode
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/ukui-build-tools-config-version.cmake.in"
    "${CMAKE_BINARY_DIR}/ukui-build-tools-config-version.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/install/ukui-build-tools-config.cmake"
    "${CMAKE_BINARY_DIR}/ukui-build-tools-config-version.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(
    FILES ${toInstallFindModules}
    DESTINATION "${FIND_MODULES_INSTALL_DIR}"
)

install(
    FILES
        ${toInstallModules}
        "${CMAKE_CURRENT_BINARY_DIR}/install/UKUiConfigVars.cmake"
    DESTINATION
        "${MODULES_INSTALL_DIR}"
)

# Tools install
install(
    PROGRAMS
        "ukui-transupdate"
    DESTINATION
        "${CMAKE_INSTALL_BINDIR}")
