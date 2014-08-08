
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
    tabletinfo.cpp
    tabletinformation.cpp
    tabletprofile.cpp
    tabletprofileconfigadaptor.cpp
    x11info.cpp
    x11input.cpp
    x11inputdevice.cpp
    x11wacom.cpp
)


add_library( wacom_common STATIC ${wacom_common_SRC})

#target_link_libraries( wacom_common ${KDE4_KDECORE_LIBS} ${LIBWACOM_LIBRARIES} )
target_link_libraries( wacom_common ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} )
set_target_properties( wacom_common PROPERTIES COMPILE_FLAGS "-fPIC" )
