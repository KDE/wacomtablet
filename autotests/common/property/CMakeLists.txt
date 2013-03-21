

kde4_add_unit_test(Common.Property
  testproperty.cpp
)

target_link_libraries(Common.Property

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
