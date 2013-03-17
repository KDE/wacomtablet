

kde4_add_unit_test(Common.PropertySet
  testpropertyset.cpp
)

target_link_libraries(Common.PropertySet

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
