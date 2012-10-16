cmake_minimum_required(VERSION 2.6)

project(wacomtablet)

## set this to false to disable automatic test execution
set(ENABLE_TESTING false)

# search packages used by KDE
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

# local cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/modules
)

#find_package(LibWacom REQUIRED)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT_STRICT_ITERATORS )
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions( -DDISABLE_NEPOMUK_LEGACY )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

#include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${LIBWACOM_INCLUDE_DIRS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

add_subdirectory( src )
add_subdirectory( data )
add_subdirectory( images )
add_subdirectory( doc )
add_subdirectory( tests )
