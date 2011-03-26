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
     screenwidget.cpp
     selectkeybutton.cpp
     selectkeystroke.cpp
     selectquotetext.cpp
     tabletwidget.cpp
)

kde4_add_ui_files( kcm_tablet_SRCS
                   errorwidget.ui
                   generalwidget.ui
                   padmapping.ui
                   penwidget.ui
                   padbuttonwidget.ui
                   presscurvedialog.ui
                   saveprofile.ui
                   screenwidget.ui
                   selectkeybutton.ui
                   selectkeystroke.ui
                   selectquotetext.ui
                   tabletwidget.ui
)

kde4_add_plugin(kcm_wacomtablet ${kcm_tablet_SRCS})

target_link_libraries( kcm_wacomtablet  
                       ${KDE4_KDECORE_LIBS}
                       ${KDE4_KDEUI_LIBS}
                       ${KDE4_KIO_LIBS}
)

install(TARGETS kcm_wacomtablet DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES kcm_wacomtablet.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
