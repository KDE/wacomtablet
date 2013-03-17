

kde4_add_unit_test(Common.Enum
  testenum.cpp
)

target_link_libraries(Common.Enum

# include static libs
        wacom_common

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
