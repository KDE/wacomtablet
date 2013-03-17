

kde4_add_unit_test(Common.DeviceProfile
  testdeviceprofile.cpp
  ../commontestutils.cpp
)

target_link_libraries(Common.DeviceProfile

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
