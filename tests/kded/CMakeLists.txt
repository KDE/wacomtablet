
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

set(TEST_MOC_HEADERS tablethandlermock.h)
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
