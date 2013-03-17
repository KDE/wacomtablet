

kde4_add_unit_test(Common.TabletProfileConfigAdaptor
  testtabletprofileconfigadaptor.cpp
  ../commontestutils.cpp
)

target_link_libraries(Common.TabletProfileConfigAdaptor

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
