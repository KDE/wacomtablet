

kde4_add_unit_test(KDED.TabletDatabase
  testtabletdatabase.cpp
  ../kdedtestutils.cpp
)

target_link_libraries(KDED.TabletDatabase

# include static libs
        wacom_common
        kded_wacomtablet_lib
        ${kded_wacomtablet_LIBS}

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)

configure_file(testtabletdatabase.companylist ${CMAKE_CURRENT_BINARY_DIR}/testtabletdatabase.companylist COPYONLY)
configure_file(testtabletdatabase.default_devicelist ${CMAKE_CURRENT_BINARY_DIR}/testtabletdatabase.default_devicelist COPYONLY)
configure_file(testtabletdatabase.aiptek_devicelist ${CMAKE_CURRENT_BINARY_DIR}/testtabletdatabase.aiptek_devicelist COPYONLY)
configure_file(testtabletdatabase.toshiba_devicelist ${CMAKE_CURRENT_BINARY_DIR}/testtabletdatabase.toshiba_devicelist COPYONLY)
configure_file(testtabletdatabase.wacom_devicelist ${CMAKE_CURRENT_BINARY_DIR}/testtabletdatabase.wacom_devicelist COPYONLY)
