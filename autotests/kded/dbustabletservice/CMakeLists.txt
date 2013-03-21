

kde4_add_unit_test(KDED.DBusTabletService
  testdbustabletservice.cpp
  ../tablethandlermock.cpp
)

target_link_libraries(KDED.DBusTabletService

# include static libs
        wacom_common
        kded_wacomtablet_lib
        ${kded_wacomtablet_LIBS}

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
