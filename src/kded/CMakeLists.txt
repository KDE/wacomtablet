
## libraries
set(kded_wacomtablet_LIBS
   wacom_common
   ${KDE4_KDECORE_LIBS}
   ${KDE4_KDEUI_LIBS}
   ${KDE4_SOLID_LIBS}
   ${X11_X11_LIB}
   ${X11_Xinput_LIB}
   ${X11_Xrandr_LIB}
   ${X11_LIBRARIES}
)

## sources
set(kded_wacomtablet_SRCS
    dbustabletservice.cpp
    tabletdaemon.cpp
    tablethandler.cpp
    xdeviceeventnotifier.cpp
    xinputadaptor.cpp
    xinputproperty.cpp
    xsetwacomadaptor.cpp
    xsetwacominterface.cpp
    xsetwacomproperty.cpp
)

## dbus interfaces
qt4_add_dbus_adaptor(kded_wacomtablet_SRCS org.kde.Wacom.xml dbustabletservice.h Wacom::DBusTabletService)


## build KDE daemon module
kde4_add_plugin(kded_wacomtablet ${kded_wacomtablet_SRCS})
target_link_libraries( kded_wacomtablet ${kded_wacomtablet_LIBS} )


## build static library for unit testing
add_library( kded_wacomtablet_lib STATIC ${kded_wacomtablet_SRCS} )
target_link_libraries( kded_wacomtablet_lib ${kded_wacomtablet_LIBS} )

if( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  set_target_properties( kded_wacomtablet_lib PROPERTIES COMPILE_FLAGS "-fPIC" )
endif( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )


## files to install
install(TARGETS kded_wacomtablet DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES wacomtablet.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install(FILES wacomtablet.notifyrc DESTINATION "${DATA_INSTALL_DIR}/wacomtablet")

install(FILES org.kde.Wacom.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})
