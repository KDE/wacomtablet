cmake_minimum_required(VERSION 3.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_policy(SET CMP0048 NEW)
project(wacomtablet VERSION "3.3.0")

set(QT_MIN_VERSION "5.7.0")
find_package(ECM 1.6.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_SOURCE_DIR}/cmake/modules)
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

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets DBus X11Extras Qml)
find_package(KF5 REQUIRED COMPONENTS CoreAddons I18n GlobalAccel Config XmlGui WidgetsAddons WindowSystem Notifications DBusAddons Plasma DocTools)
find_package(XCB REQUIRED COMPONENTS XINPUT)
find_package(X11 REQUIRED)
find_package(XorgWacom REQUIRED)
find_package(LibWacom REQUIRED)

set(USING_X_LIBRARIES XCB::XINPUT ${X11_Xinput_LIB})

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
add_subdirectory( doc )

### Tests
if(BUILD_TESTING)
  find_package(Qt5Test ${QT_MIN_VERSION} CONFIG REQUIRED)
  add_subdirectory( autotests )
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
