

kde4_add_unit_test(Common.DeviceInformation
  testdeviceinformation.cpp
  ../commontestutils.cpp
)

target_link_libraries(Common.DeviceInformation

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
