
## libraries
set(kded_wacomtablet_LIBS
   wacom_common
   ${KDE4_KDECORE_LIBS}
   ${KDE4_KDEUI_LIBS}
   ${X11_X11_LIB}
   ${X11_Xinput_LIB}
   ${X11_Xrandr_LIB}
   ${X11_LIBRARIES}
)

## sources
set(kded_wacomtablet_SRCS
    dbustabletservice.cpp
    eventnotifier.cpp
    procsystemadaptor.cpp
    procsystemproperty.cpp
    tabletbackend.cpp
    tabletbackendfactory.cpp
    tabletdaemon.cpp
    tabletdatabase.cpp
    tabletfinder.cpp
    tablethandler.cpp
    x11eventnotifier.cpp
    x11tabletfinder.cpp
    xinputadaptor.cpp
    xinputproperty.cpp
    xsetwacomadaptor.cpp
    xsetwacomproperty.cpp
)

## dbus interfaces
qt4_add_dbus_adaptor(kded_wacomtablet_SRCS org.kde.Wacom.xml dbustabletservice.h Wacom::DBusTabletService)

## build KDE daemon module
kde4_add_plugin(kded_wacomtablet ${kded_wacomtablet_SRCS})
target_link_libraries( kded_wacomtablet ${kded_wacomtablet_LIBS} )

## files to install
install(TARGETS kded_wacomtablet DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES wacomtablet.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install(FILES wacomtablet.notifyrc DESTINATION "${DATA_INSTALL_DIR}/wacomtablet")

install(FILES org.kde.Wacom.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})


## LIBRARY FOR UNIT TESTS
if (KDE4_BUILD_TESTS)

    ## headers which need to be moc'ed for the static unit-testing library
    set(kded_wacomtablet_HDRS
        dbustabletservice.h
        tabletdaemon.h
        tablethandler.h
        eventnotifier.h
        x11eventnotifier.h
    )

    ## create cpp mocs
    qt4_wrap_cpp (kded_wacomtablet_MOCS ${kded_wacomtablet_HDRS})

    ## build static library for unit testing
    add_library( kded_wacomtablet_lib STATIC ${kded_wacomtablet_SRCS} ${kded_wacomtablet_MOCS} )
    target_link_libraries( kded_wacomtablet_lib ${kded_wacomtablet_LIBS} )
    set_target_properties( kded_wacomtablet_lib PROPERTIES COMPILE_FLAGS "-fPIC" )

endif (KDE4_BUILD_TESTS)
