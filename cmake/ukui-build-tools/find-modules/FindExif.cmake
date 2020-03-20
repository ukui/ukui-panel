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

#.rst:
# FindExif
# -----------
#
# Try to find the Exif library
#
# Once done this will define
#
# ::
#
#   EXIF_FOUND - System has the Exif library
#   EXIF_INCLUDE_DIR - The Exif library include directory
#   EXIF_INCLUDE_DIRS - Location of the headers needed to use the Exif library
#   EXIF_LIBRARIES - The libraries needed to use the Exif library
#   EXIF_DEFINITIONS - Compiler switches required for using the Exif library
#   EXIF_VERSION_STRING - the version of the Exif library found


# use pkg-config to get the directories and then use these values
# in the find_path() and find_library() calls
find_package(PkgConfig)
pkg_check_modules(PC_EXIF libexif)
set(EXIF_DEFINITIONS ${PC_EXIF_CFLAGS_OTHER})

find_path(EXIF_INCLUDE_DIR NAMES libexif/exif-data.h
   HINTS
   ${PC_EXIF_INCLUDEDIR}
   ${PC_EXIF_INCLUDE_DIRS}
   PATH_SUFFIXES libexif
)

find_library(EXIF_LIBRARIES NAMES exif libexif
   HINTS
   ${PC_EXIF_LIBDIR}
   ${PC_EXIF_LIBRARY_DIRS}
)


# iterate over all dependencies
unset(FD_LIBRARIES)
foreach(depend ${PC_EXIF_LIBRARIES})
    find_library(_DEPEND_LIBRARIES
        NAMES
            ${depend}
        HINTS
            ${PC_EXIF_LIBDIR}
            ${PC_EXIF_LIBRARY_DIRS}
    )

    if (_DEPEND_LIBRARIES)
        list(APPEND FD_LIBRARIES ${_DEPEND_LIBRARIES})
    endif()
    unset(_DEPEND_LIBRARIES CACHE)
endforeach()

set(EXIF_VERSION_STRING ${PC_EXIF_VERSION})
set(EXIF_INCLUDE_DIR ${PC_EXIF_INCLUDEDIR})

list(APPEND EXIF_INCLUDE_DIRS
    ${EXIF_INCLUDE_DIR}
    ${PC_EXIF_INCLUDE_DIRS}
)
list(REMOVE_DUPLICATES EXIF_INCLUDE_DIRS)

list(APPEND EXIF_LIBRARIES
    ${FD_LIBRARIES}
)

list(REMOVE_DUPLICATES EXIF_LIBRARIES)
# handle the QUIETLY and REQUIRED arguments and set EXIF_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Exif
                                  REQUIRED_VARS EXIF_LIBRARIES EXIF_INCLUDE_DIR EXIF_INCLUDE_DIRS
                                  VERSION_VAR EXIF_VERSION_STRING)

mark_as_advanced(EXIF_INCLUDE_DIR EXIF_LIBRARIES)
