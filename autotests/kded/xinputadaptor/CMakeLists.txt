

kde4_add_unit_test(KDED.XinputWacomAdaptor
  testxinputadaptor.cpp
  ../tabletdependenttest.cpp
)

target_link_libraries(KDED.XinputWacomAdaptor

# include static libs
        wacom_common
        kded_wacomtablet_lib
        ${kded_wacomtablet_LIBS}

# include general qt/kde libs
        ${QT_QTTEST_LIBRARY}
)
