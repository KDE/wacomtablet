# - Try to find LibWacom
# Once done this will define
#  LIBWACOM_FOUND        - System has LibWacom
#  LIBWACOM_INCLUDE_DIRS - The LibWacom include directories
#  LIBWACOM_LIBRARIES    - The libraries needed to use LibWacom
#  LIBWACOM_DEFINITIONS  - Compiler switches required for using LibWacom

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

set(LIBWACOM_LIBRARIES ${LIBWACOM_LIBRARY} )
set(LIBWACOM_INCLUDE_DIRS ${LIBWACOM_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBWACOM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBWACOM  DEFAULT_MSG
                                  LIBWACOM_LIBRARY LIBWACOM_INCLUDE_DIR)

mark_as_advanced(LIBWACOM_INCLUDE_DIR LIBWACOM_LIBRARY )
