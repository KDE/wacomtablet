cmake_minimum_required(VERSION 3.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_policy(SET CMP0048 NEW)
project(wacomtablet VERSION "3.3.0")

set(QT_MIN_VERSION "5.7.0")
find_package(ECM 5.80.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(ECMOptionalAddSubdirectory)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX WACOMTABLET
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/wacomtablet-version.h"
)

find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS Core Gui Widgets DBus Qml)
find_package(KF5 REQUIRED COMPONENTS CoreAddons I18n GlobalAccel Config XmlGui WidgetsAddons WindowSystem Notifications DBusAddons Plasma DocTools KCMUtils KIO)
find_package(XCB REQUIRED COMPONENTS XINPUT)
find_package(X11 REQUIRED)

if(QT_MAJOR_VERSION STREQUAL "6")
  find_package(KF6Plasma5Support REQUIRED)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBWACOM libwacom REQUIRED IMPORTED_TARGET)
pkg_check_modules(XORGWACOM xorg-wacom REQUIRED IMPORTED_TARGET)

if(QT_MAJOR_VERSION EQUAL "5")
  find_package(Qt5X11Extras REQUIRED)
endif()

if(${LIBWACOM_VERSION} VERSION_LESS "0.29")
    message(STATUS "Button detection with libwacom requires version at least 0.29. Detected version is: " ${LIBWACOM_VERSION})
    add_definitions(-DLIBWACOM_EVDEV_MISSING)
endif()

add_definitions( -DQT_STRICT_ITERATORS )
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions( -DQT_USE_QSTRINGBUILDER )

add_subdirectory( src )
add_subdirectory( data )
add_subdirectory( images )

ki18n_install(po)
if(KF5DocTools_FOUND)
    add_subdirectory( doc )
    kdoctools_install(po)
endif()

### Tests
if(BUILD_TESTING)
  find_package(Qt${QT_MAJOR_VERSION}Test ${QT_MIN_VERSION} CONFIG REQUIRED)
  add_subdirectory( autotests )
endif()
ecm_qt_install_logging_categories(
        EXPORT WACOMTABLET
        FILE wacomtablet.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
