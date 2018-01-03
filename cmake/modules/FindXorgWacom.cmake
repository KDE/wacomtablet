# - Try to find xserver-xorg-input-wacom
# Once done this will define
#  XORGWACOM_FOUND        - System has xserver-xorg-input-wacom
#  XORGWACOM_INCLUDE_DIRS - The xserver-xorg-input-wacom include directories
#  XORGWACOM_LIBRARIES    - The libraries needed to use xserver-xorg-input-wacom
#  XORGWACOM_DEFINITIONS  - Compiler switches required for using LibWacom
#
# Copyright (c) 2013  Jörg Ehrichs <Joerg.Ehrichs@gmx.de>
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

pkg_search_module(XORGWACOM_PKGCONF xorg-wacom)

set(XORGWACOM_DEFINITIONS ${XORGWACOM_PKGCONF_CFLAGS_OTHER})

find_path(XORGWACOM_INCLUDE_DIR 
          NAMES xorg/wacom-properties.h
          HINTS ${XORGWACOM_PKGCONF_INCLUDEDIR} ${XORGWACOM_PKGCONF_INCLUDE_DIRS}
          PATH_SUFFIXES libwacom )

#find_library(XORGWACOM_LIBRARY 
#             NAMES wacom libwacom
#             HINTS ${XORGWACOM_PKGCONF_LIBDIR} ${XORGWACOM_PKGCONF_LIBRARY_DIRS} )

#set(XORGWACOM_LIBRARIES ${XORGWACOM_LIBRARY} )
set(XORGWACOM_INCLUDE_DIRS ${XORGWACOM_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set XORGWACOM_FOUND to TRUE
# if all listed variables are TRUE
#find_package_handle_standard_args(XORGWACOM  DEFAULT_MSG
#                                  XORGWACOM_LIBRARY XORGWACOM_INCLUDE_DIR)
find_package_handle_standard_args(XORGWACOM  DEFAULT_MSG
                                  XORGWACOM_INCLUDE_DIR)

#mark_as_advanced(XORGWACOM_INCLUDE_DIR XORGWACOM_LIBRARY )
mark_as_advanced(XORGWACOM_INCLUDE_DIR )
