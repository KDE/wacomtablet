########### next target ###############

set( kcm_tablet_SRCS
     areaselectionwidget.cpp
     buttonactiondisplaywidget.cpp
     buttonactionselectiondialog.cpp
     buttonactionselectionwidget.cpp
     buttonactionselectorwidget.cpp
     buttonpagewidget.cpp
     calibrationdialog.cpp
     generalpagewidget.cpp
     kcmwacomtablet.cpp
     kcmwacomtabletwidget.cpp
     keysequenceinputbutton.cpp
     keysequenceinputwidget.cpp
     pressurecurvewidget.cpp
     pressurecurvedialog.cpp
     styluspagewidget.cpp
     tabletareaselectioncontroller.cpp
     tabletareaselectiondialog.cpp
     tabletareaselectionview.cpp
     tabletareaselectionwidget.cpp
     tabletpagewidget.cpp
     touchpagewidget.cpp
)

ki18n_wrap_ui( kcm_tablet_SRCS
                   buttonactionselectionwidget.ui
                   buttonactionselectorwidget.ui
                   buttonpagewidget.ui
                   errorwidget.ui
                   generalpagewidget.ui
                   kcmwacomtabletwidget.ui
                   pressurecurvedialog.ui
                   saveprofile.ui
                   styluspagewidget.ui
                   tabletareaselectionview.ui
                   tabletpagewidget.ui
                   touchpagewidget.ui
)

add_definitions(-DTRANSLATION_DOMAIN=\"wacomtablet\")
add_library(kcm_wacomtablet MODULE ${kcm_tablet_SRCS})

target_link_libraries( kcm_wacomtablet
                       wacom_common
                       Qt5::Core
                       Qt5::Widgets
                       Qt5::X11Extras
                       KF5::CoreAddons
                       KF5::XmlGui
                       KF5::WidgetsAddons
                       KF5::WindowSystem
                       ${X11_X11_LIB}
                       ${X11_Xinput_LIB}
                       ${X11_LIBRARIES}
                       ${X11_Xrandr_LIB}
)

install(TARGETS kcm_wacomtablet DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES kcm_wacomtablet.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
