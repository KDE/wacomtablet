
set(wacom_tablet_SRCS
    dbustabletservice.cpp
    wacominterface.cpp
    tabletdaemon.cpp
    tablethandler.cpp
    xdeviceeventnotifier.cpp
    xinputadaptor.cpp
    xinputproperty.cpp
    xsetwacomadaptor.cpp
    xsetwacomproperty.cpp
)

qt4_add_dbus_adaptor(wacom_tablet_SRCS org.kde.Wacom.xml dbustabletservice.h Wacom::DBusTabletService)
kde4_add_plugin(kded_wacomtablet ${wacom_tablet_SRCS})

target_link_libraries( kded_wacomtablet
                       wacom_common
                       ${KDE4_KDECORE_LIBS}
                       ${KDE4_KDEUI_LIBS}
                       ${KDE4_SOLID_LIBS}
                       ${X11_X11_LIB}
                       ${X11_Xinput_LIB}
                       ${X11_Xrandr_LIB}
                       ${X11_LIBRARIES}
)

install(TARGETS kded_wacomtablet DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES wacomtablet.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install(FILES wacomtablet.notifyrc DESTINATION "${DATA_INSTALL_DIR}/wacomtablet")

install(FILES org.kde.Wacom.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})
