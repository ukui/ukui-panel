# - Try to find the UDev library
# Once done this will define
#
#  UDEV_FOUND - system has UDev
#  UDEV_INCLUDE_DIR - the libudev include directory
#  UDEV_LIBS - The libudev libraries

# Copyright (c) 2010, Rafael Fernández López, <ereslibre@kde.org>
# Copyright (c) 2016, Luís Pereira, <luis.artur.pereira@gmail.com>
# Copyright (c) 2019 Tianjin KYLIN Information Technology Co., Ltd. *
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.


MACRO (BUILD_UKUI_PLUGIN NAME)
    set(PROGRAM "ukui-panel")
    project(${PROGRAM}_${NAME})

    set(PROG_SHARE_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/ukui/${PROGRAM})
    set(PLUGIN_SHARE_DIR ${PROG_SHARE_DIR}/${NAME})

    # Translations **********************************
#    ukui_translate_ts(${PROJECT_NAME}_QM_FILES
#       UPDATE_TRANSLATIONS ${UPDATE_TRANSLATIONS}
#        SOURCES
#            ${HEADERS}
#            ${SOURCES}
#            ${MOCS}
#            ${UIS}
#        TEMPLATE
#            ${NAME}
#        INSTALL_DIR
#        ${UKUI_TRANSLATIONS_DIR}/${PROGRAM}/${NAME}
#   )

    file (GLOB ${PROJECT_NAME}_DESKTOP_FILES_IN resources/*.desktop.in)
    ukui_translate_desktop(DESKTOP_FILES
        SOURCES
            ${${PROJECT_NAME}_DESKTOP_FILES_IN}
    )

    ukui_plugin_translation_loader(QM_LOADER ${NAME} "ukui-panel")
    #************************************************

    file (GLOB CONFIG_FILES resources/*.conf)

    if (NOT DEFINED PLUGIN_DIR)
        set (PLUGIN_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/${PROGRAM})
    endif (NOT DEFINED PLUGIN_DIR)

    set(QTX_LIBRARIES Qt5::Widgets)
    if(QT_USE_QTXML)
        set(QTX_LIBRARIES ${QTX_LIBRARIES} Qt5::Xml)
    endif()
    if(QT_USE_QTDBUS)
        set(QTX_LIBRARIES ${QTX_LIBRARIES} Qt5::DBus)
    endif()

    list(FIND STATIC_PLUGINS ${NAME} IS_STATIC)
    set(SRC ${HEADERS} ${SOURCES} ${QM_LOADER} ${MOC_SOURCES} ${${PROJECT_NAME}_QM_FILES} ${RESOURCES} ${UIS} ${DESKTOP_FILES})
    if (${IS_STATIC} EQUAL -1) # not static
        add_library(${NAME} MODULE ${SRC}) # build dynamically loadable modules
        install(TARGETS ${NAME} DESTINATION ${PLUGIN_DIR}) # install the *.so file
    else() # static
        add_library(${NAME} STATIC ${SRC}) # build statically linked lib
    endif()
    #target_link_libraries(${NAME} ${QTX_LIBRARIES} ${LIBRARIES} KF5::WindowSystem)
    target_link_libraries(${NAME} ${QTX_LIBRARIES} ${LIBRARIES} KF5::WindowSystem)
    install(FILES ${CONFIG_FILES}  DESTINATION ${PLUGIN_SHARE_DIR})
    install(FILES ${DESKTOP_FILES} DESTINATION ${PROG_SHARE_DIR})



ENDMACRO(BUILD_UKUI_PLUGIN)
