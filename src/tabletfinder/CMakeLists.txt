
set( tabletfinder_SRCS
     main.cpp
     dialog.cpp
     hwbuttondialog.cpp
)

ki18n_wrap_ui( tabletfinder_SRCS
                   dialog.ui
                   hwbuttondialog.ui
)

add_executable(kde_wacom_tabletfinder ${tabletfinder_SRCS})

target_link_libraries( kde_wacom_tabletfinder
                       wacom_common
                       ${KDE4_KDECORE_LIBS}
                       ${KDE4_KDEUI_LIBS}
                       ${X11_X11_LIB}
                       ${X11_Xinput_LIB}
                       ${X11_LIBRARIES}
)

install(TARGETS kde_wacom_tabletfinder DESTINATION ${BIN_INSTALL_DIR} )
install( FILES kde_wacom_tabletfinder.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
