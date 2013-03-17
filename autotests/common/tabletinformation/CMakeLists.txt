

kde4_add_unit_test(Common.TabletInformation
  testtabletinformation.cpp
  ../commontestutils.cpp
)

target_link_libraries(Common.TabletInformation

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
