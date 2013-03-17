

kde4_add_unit_test(Common.ProfileManager
  testprofilemanager.cpp
  ../commontestutils.cpp
)

target_link_libraries(Common.ProfileManager

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
