cmake_minimum_required(VERSION 3.0)

project(wacomtablet)

set(QT_MIN_VERSION "5.3.0")
find_package(ECM 1.6.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMOptionalAddSubdirectory)
include(ECMInstallIcons)

find_package(ECM REQUIRED NOCOMPONENTS)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets DBus X11Extras Qml)
find_package(KF5 REQUIRED COMPONENTS CoreAddons I18n GlobalAccel Config XmlGui WidgetsAddons WindowSystem Notifications DBusAddons Plasma DocTools)
find_package(XCB REQUIRED COMPONENTS XINPUT RANDR)
find_package(X11 REQUIRED)

# local cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/modules
)

find_package(XorgWacom REQUIRED)

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
