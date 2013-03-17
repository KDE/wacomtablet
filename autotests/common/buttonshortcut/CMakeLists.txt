

kde4_add_unit_test(Common.ButtonShortcut
  testbuttonshortcut.cpp
)

target_link_libraries(Common.ButtonShortcut

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
