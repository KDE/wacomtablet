cmake_minimum_required(VERSION 3.0)

cmake_policy(SET CMP0048 NEW)
project(wacomtablet VERSION "3.0.0")

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

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX WACOMTABLET
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/wacomtablet-version.h"
)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets DBus X11Extras Qml)
find_package(KF5 REQUIRED COMPONENTS CoreAddons I18n GlobalAccel Config XmlGui WidgetsAddons WindowSystem Notifications DBusAddons Plasma DocTools)
find_package(XCB OPTIONAL_COMPONENTS XINPUT)
find_package(X11 REQUIRED COMPONENTS XLIB)
find_package(XorgWacom REQUIRED)
find_package(LibWacom REQUIRED)

# xcb-xinput is an experimental API and is not built by default in the xcb
# repository. Downstream distributions without xcb-xinput, such as Kubuntu, can
# build using the Xlib version of the library instead.
#
# Because Qt5 uses xcb exclusively for native event handling, kded is unable
# to monitor hotplugging events, since these are signaled as xinput events.
#
# Users can enable xcb-xinput by building xcb with the configuration flag --enable-xinput.
if(XCB_XINPUT_FOUND)
    message(STATUS "Using XCB_XINPUT.  Please note this is an unstable API.")
    set(USING_X_LIBRARIES XCB::XINPUT ${X11_Xinput_LIB})
    add_definitions(-DHAVE_XCB_XINPUT)
else()
    message(STATUS "Falling back to X11_XINPUT.")
    set(USING_X_LIBRARIES ${X11_LIBRARIES} ${X11_Xinput_LIB})
endif()

add_definitions( -DQT_STRICT_ITERATORS )
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions( -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS )

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
