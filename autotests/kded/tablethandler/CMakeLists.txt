

kde4_add_unit_test(KDED.TabletHandler
  testtablethandler.cpp
  ../kdedtestutils.cpp
  ../tabletbackendmock.cpp
)

target_link_libraries(KDED.TabletHandler

# include static libs
        wacom_common
        kded_wacomtablet_lib
        ${kded_wacomtablet_LIBS}

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)

configure_file(testtablethandler.configrc ${CMAKE_CURRENT_BINARY_DIR}/testtablethandler.configrc COPYONLY)
configure_file(testtablethandler.profilesrc ${CMAKE_CURRENT_BINARY_DIR}/testtablethandler.profilesrc COPYONLY)