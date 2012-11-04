
## include directories for unit tests
set(TEST_INCLUDES
    ../../src
    ../../src/applet
    ../../src/kcmodule
    ../../src/kded
    ../../src/common
    ${QT_INCLUDES}
    ${KDE4_INCLUDES})

## additional sources which are required for the tests
set(TEST_SOURCES commontestutils.cpp)

## libraries for unit tests
set(TEST_LIBRARIES wacom_common ${KDE4_KDECORE_LIBS})

## set includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${TEST_INCLUDES})

add_qttest(testenum testenum.cpp ${TEST_SOURCES})
add_qttest(testproperty testproperty.cpp ${TEST_SOURCES})
add_qttest(testpropertyset testpropertyset.cpp ${TEST_SOURCES})
add_qttest(testdeviceproperty testdeviceproperty.cpp ${TEST_SOURCES})
add_qttest(testdeviceprofile testdeviceprofile.cpp ${TEST_SOURCES})
add_qttest(testtabletprofile testtabletprofile.cpp ${TEST_SOURCES})
add_qttest(testdeviceinformation testdeviceinformation.cpp ${TEST_SOURCES})
add_qttest(testdeviceprofileconfigadaptor testdeviceprofileconfigadaptor.cpp ${TEST_SOURCES})
add_qttest(testtabletinformation testtabletinformation.cpp ${TEST_SOURCES})
add_qttest(testtabletprofileconfigadaptor testtabletprofileconfigadaptor.cpp ${TEST_SOURCES})
add_qttest(testprofilemanager testprofilemanager.cpp ${TEST_SOURCES})

#add_qttest(testtest testtest.cpp ${TEST_SOURCES})
