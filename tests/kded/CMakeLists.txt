
## include directories for unit tests
set(TEST_INCLUDES
    ../../src
    ../../src/applet
    ../../src/kcmodule
    ../../src/kded
    ../../src/common
    ${QT_INCLUDES}
    ${KDE4_INCLUDES})

## additional sources and qt-mocs which are required for the tests
set(TEST_SOURCES
    kdedtestutils.cpp
    tabletbackendmock.cpp
    tablethandlermock.cpp
    tabletdependenttest.cpp
)

set(TEST_MOC_HEADERS
    tabletdependenttest.h
    tablethandlermock.h)

qt4_wrap_cpp (TEST_MOC_FILES ${TEST_MOC_HEADERS})


## libraries for unit tests
set(TEST_LIBRARIES
    wacom_common
    kded_wacomtablet_lib
    ${kded_wacomtablet_LIBS}
)

## set includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${TEST_INCLUDES})

add_qttest(testdbustabletservice testdbustabletservice.cpp ${TEST_SOURCES} ${TEST_MOC_FILES})
add_qttest(testtabletbackend testtabletbackend.cpp ${TEST_SOURCES} ${TEST_MOC_FILES})

add_qttest(testtabletdatabase testtabletdatabase.cpp ${TEST_SOURCES} ${TEST_MOC_FILES})
configure_file(testtabletdatabase.companylist ${CMAKE_CURRENT_BINARY_DIR}/testtabletdatabase.companylist COPYONLY)
configure_file(testtabletdatabase.aiptek_devicelist ${CMAKE_CURRENT_BINARY_DIR}/testtabletdatabase.aiptek_devicelist COPYONLY)
configure_file(testtabletdatabase.toshiba_devicelist ${CMAKE_CURRENT_BINARY_DIR}/testtabletdatabase.toshiba_devicelist COPYONLY)
configure_file(testtabletdatabase.wacom_devicelist ${CMAKE_CURRENT_BINARY_DIR}/testtabletdatabase.wacom_devicelist COPYONLY)

add_qttest(testtablethandler testtablethandler.cpp ${TEST_SOURCES} ${TEST_MOC_FILES})
configure_file(testtablethandler.configrc ${CMAKE_CURRENT_BINARY_DIR}/testtablethandler.configrc COPYONLY)
configure_file(testtablethandler.profilesrc ${CMAKE_CURRENT_BINARY_DIR}/testtablethandler.profilesrc COPYONLY)

add_qttest(testxinputadaptor testxinputadaptor.cpp ${TEST_SOURCES} ${TEST_MOC_FILES})
add_qttest(testxsetwacomadaptor testxsetwacomadaptor.cpp ${TEST_SOURCES} ${TEST_MOC_FILES})
