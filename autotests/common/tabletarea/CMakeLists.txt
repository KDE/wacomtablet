kde4_add_unit_test(Common.TabletArea
  testtabletarea.cpp
)

target_link_libraries(Common.TabletArea

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
