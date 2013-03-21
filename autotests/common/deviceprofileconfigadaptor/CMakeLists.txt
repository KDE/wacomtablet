

kde4_add_unit_test(Common.DeviceProfileConfigAdaptor
  testdeviceprofileconfigadaptor.cpp
  ../commontestutils.cpp
)

target_link_libraries(Common.DeviceProfileConfigAdaptor

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
