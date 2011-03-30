########### next target ###############

set( kcm_tablet_SRCS
     generalwidget.cpp
     kcmwacomtablet.cpp
     padmapping.cpp
     padbuttonwidget.cpp
     penwidget.cpp
     presscurve.cpp
     presscurvedialog.cpp
     profilemanagement.cpp
     selectkeybutton.cpp
     selectkeystroke.cpp
     tabletwidget.cpp
     calibrationdialog.cpp
     touchwidget.cpp
)

kde4_add_ui_files( kcm_tablet_SRCS
                   errorwidget.ui
                   generalwidget.ui
                   padmapping.ui
                   penwidget.ui
                   padbuttonwidget.ui
                   presscurvedialog.ui
                   saveprofile.ui
                   selectkeybutton.ui
                   selectkeystroke.ui
                   tabletwidget.ui
                   touchwidget.ui
)

kde4_add_plugin(kcm_wacomtablet ${kcm_tablet_SRCS})

target_link_libraries( kcm_wacomtablet  
                       ${KDE4_KDECORE_LIBS}
                       ${KDE4_KDEUI_LIBS}
                       ${KDE4_KIO_LIBS}
                       ${X11_X11_LIB}
                       ${X11_Xinput_LIB}
                       ${X11_LIBRARIES}
                       ${X11_Xrandr_LIB}
)

install(TARGETS kcm_wacomtablet DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES kcm_wacomtablet.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
