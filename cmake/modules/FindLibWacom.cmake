# - Try to find LibWacom
# Once done this will define
#  LIBWACOM_FOUND        - System has LibWacom
#  LIBWACOM_INCLUDE_DIRS - The LibWacom include directories
#  LIBWACOM_LIBRARIES    - The libraries needed to use LibWacom
#  LIBWACOM_DEFINITIONS  - Compiler switches required for using LibWacom
#
# Copyright (c) 2012  Alexander Maret-Huskinson <alex@maret.de>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
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

find_package(PkgConfig)

pkg_search_module(LIBWACOM_PKGCONF libwacom)

set(LIBWACOM_DEFINITIONS ${LIBWACOM_PKGCONF_CFLAGS_OTHER})

find_path(LIBWACOM_INCLUDE_DIR 
          NAMES libwacom/libwacom.h
          HINTS ${LIBWACOM_PKGCONF_INCLUDEDIR} ${LIBWACOM_PKGCONF_INCLUDE_DIRS}
          PATH_SUFFIXES libwacom )

find_library(LIBWACOM_LIBRARY 
             NAMES wacom libwacom
             HINTS ${LIBWACOM_PKGCONF_LIBDIR} ${LIBWACOM_PKGCONF_LIBRARY_DIRS} )

set(LIBWACOM_LIBRARIES ${LIBWACOM_LIBRARY})
set(LIBWACOM_INCLUDE_DIRS ${LIBWACOM_INCLUDE_DIR})
set(LIBWACOM_VERSION ${LIBWACOM_PKGCONF_VERSION})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBWACOM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBWACOM
    REQUIRED_VARS LIBWACOM_LIBRARY LIBWACOM_INCLUDE_DIR
    VERSION_VAR LIBWACOM_VERSION)

mark_as_advanced(LIBWACOM_INCLUDE_DIR LIBWACOM_LIBRARY)
