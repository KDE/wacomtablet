
## libraries
set(kded_wacomtablet_LIBS
   wacom_common
   KF5::CoreAddons
   KF5::Notifications
   KF5::XmlGui
   KF5::DBusAddons
   KF5::ConfigCore
   XCB::XINPUT
   X11::Xi
   PkgConfig::LIBWACOM
)

## sources

# this file contains plugin definition and does not belong in kded_wacomtablet_lib
# don't forget to add extra kcoreaddons_desktop_to_json for it if you wan to include it
set(kded_wacomtablet_PLUGIN_SRC
    tabletdaemon.cpp
)

set(kded_wacomtablet_SRCS
    dbustabletservice.cpp
    eventnotifier.cpp
    procsystemadaptor.cpp
    procsystemproperty.cpp
    tabletbackend.cpp
    tabletbackendfactory.cpp
    tabletfinder.cpp
    tablethandler.cpp
    x11eventnotifier.cpp
    x11tabletfinder.cpp
    xinputadaptor.cpp
    xinputproperty.cpp
    xsetwacomadaptor.cpp
    xsetwacomproperty.cpp
    ../common/globalactions.cpp
)

ecm_qt_declare_logging_category(
    kded_wacomtablet_SRCS
    HEADER "logging.h"
    IDENTIFIER "KDED"
    CATEGORY_NAME "org.kde.wacomtablet.kded")

## dbus interfaces
qt5_add_dbus_adaptor(kded_wacomtablet_SRCS org.kde.Wacom.xml dbustabletservice.h Wacom::DBusTabletService)

## build KDE daemon module
add_definitions(-DTRANSLATION_DOMAIN=\"wacomtablet\")
add_library(kded_wacomtablet MODULE ${kded_wacomtablet_PLUGIN_SRC} ${kded_wacomtablet_SRCS})
target_link_libraries( kded_wacomtablet ${kded_wacomtablet_LIBS} )

set_target_properties(kded_wacomtablet PROPERTIES OUTPUT_NAME wacomtablet)

## files to install
kcoreaddons_desktop_to_json(kded_wacomtablet wacomtablet.desktop)

install(TARGETS kded_wacomtablet DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kded)
install(FILES wacomtablet.notifyrc DESTINATION "${KDE_INSTALL_KNOTIFY5RCDIR}")
install(FILES org.kde.Wacom.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES wacomtablet.categories DESTINATION ${KDE_INSTALL_CONFDIR})

## LIBRARY FOR UNIT TESTS
if (BUILD_TESTING)
    ## build static library for unit testing
    add_library( kded_wacomtablet_lib STATIC ${kded_wacomtablet_SRCS} )
    target_link_libraries( kded_wacomtablet_lib ${kded_wacomtablet_LIBS} )
    set_target_properties( kded_wacomtablet_lib PROPERTIES COMPILE_FLAGS "-fPIC" )

endif (BUILD_TESTING)
