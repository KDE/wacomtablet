


set(wacom_common_LIBS
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::X11Extras
  KF5::I18n
  KF5::GlobalAccel
  KF5::ConfigCore
  KF5::CoreAddons
  ${USING_X_LIBRARIES}
)

set(wacom_common_SRC
    aboutdata.cpp
    buttonshortcut.cpp
    dbustabletinterface.cpp
    deviceinformation.cpp
    deviceprofile.cpp
    deviceprofileconfigadaptor.cpp
    deviceproperty.cpp
    devicetype.cpp
    mainconfig.cpp
    profilemanager.cpp
    profilemanagement.cpp
    property.cpp
    propertyadaptor.cpp
    screenrotation.cpp
    screenmap.cpp
    screenspace.cpp
    stringutils.cpp
    tabletarea.cpp
    tabletdatabase.cpp
    tabletinfo.cpp
    tabletinformation.cpp
    tabletprofile.cpp
    tabletprofileconfigadaptor.cpp
    x11info.cpp
    x11input.cpp
    x11inputdevice.cpp
    x11wacom.cpp
)

qt5_add_dbus_interfaces(wacom_common_SRC
                        ../kded/org.kde.Wacom.xml)

add_library( wacom_common STATIC ${wacom_common_SRC})

target_link_libraries( wacom_common ${wacom_common_LIBS} )
set_target_properties( wacom_common PROPERTIES COMPILE_FLAGS "-fPIC" )
