# - Try to find xserver-xorg-input-wacom
# Once done this will define
#  XORGWACOM_FOUND        - System has xserver-xorg-input-wacom
#  XORGWACOM_INCLUDE_DIRS - The xserver-xorg-input-wacom include directories
#  XORGWACOM_LIBRARIES    - The libraries needed to use xserver-xorg-input-wacom
#  XORGWACOM_DEFINITIONS  - Compiler switches required for using LibWacom

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
