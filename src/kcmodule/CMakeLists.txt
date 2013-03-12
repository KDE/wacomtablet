########### next target ###############

set( kcm_tablet_SRCS
     areaselectionwidget.cpp
     buttonactiondisplaywidget.cpp
     buttonactionselectiondialog.cpp
     buttonactionselectionwidget.cpp
     buttonactionselectorwidget.cpp
     calibrationdialog.cpp
     generalwidget.cpp
     kcmwacomtablet.cpp
     padbuttonwidget.cpp
     penwidget.cpp
     presscurve.cpp
     presscurvedialog.cpp
     profilemanagement.cpp
     screenareaselectiondialog.cpp
     screenareaselectionwidget.cpp
     tabletareaselectiondialog.cpp
     tabletareaselectionwidget.cpp
     tabletpagewidget.cpp
     tabletwidget.cpp
)

kde4_add_ui_files( kcm_tablet_SRCS
                   buttonactionselectionwidget.ui
                   buttonactionselectorwidget.ui
                   errorwidget.ui
                   generalwidget.ui
                   padbuttonwidget.ui
                   penwidget.ui
                   presscurvedialog.ui
                   saveprofile.ui
                   screenareaselectionwidget.ui
                   tabletareaselectionwidget.ui
                   tabletpagewidget.ui
                   tabletwidget.ui
)

kde4_add_plugin(kcm_wacomtablet ${kcm_tablet_SRCS})

target_link_libraries( kcm_wacomtablet
                       wacom_common
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
