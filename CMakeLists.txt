cmake_minimum_required(VERSION 3.0)

project(wacomtablet)

set(QT_MIN_VERSION "5.3.0")
find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

find_package(ECM REQUIRED NOCOMPONENTS)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets DBus X11Extras)
find_package(KF5 REQUIRED COMPONENTS CoreAddons I18n GlobalAccel Config XmlGui WidgetsAddons WindowSystem Notifications DBusAddons )
find_package(X11 REQUIRED)

#--------------------------------------------------------------------------
# CTest setup
#--------------------------------------------------------------------------
enable_testing()
include(CTest)
include(CTestConfig.cmake)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake COPYONLY)


# local cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/modules
)

#find_package(LibWacom REQUIRED)
find_package(XorgWacom REQUIRED)

add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT_STRICT_ITERATORS )
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions( -DDISABLE_NEPOMUK_LEGACY )
add_definitions( -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS )

#include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${LIBWACOM_INCLUDE_DIRS})
include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} )

add_subdirectory( src )
