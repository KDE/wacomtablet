
## set this to false to disable automatic test execution
set(ENABLE_TESTING false)

## make sure QTTEST is available
find_package(Qt4 REQUIRED)
include( ${QT_USE_FILE} )
set(QT_USE_QTTEST TRUE)

## include directories for unit tests
set(TEST_INCLUDES
    ../src
    ../src/applet
    ../src/kcmodule
    ../src/kded
    ../src/common
    ${QT_INCLUDES}
    ${KDE4_INCLUDES})

## libraries for unit tests
set(TEST_LIBRARIES wacom_common ${KDE4_KDECORE_LIBS})

## additional sources which are required for the tests
set(TEST_SOURCES deviceprofiletestutils.cpp)


## set includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${TEST_INCLUDES})

## MACRO add_qttest
##
## @param testname The test name.
## @param testsrc  The source files of this unit test. The first element of this list will be qt4_automoc'ed!
##
## This macro only works if header files are used. Using cpp files only does not work!
##
MACRO(add_qttest testname testsrc)
    if( ENABLE_TESTING )
        SET(${testname}_SRC ${testsrc})
        foreach(src ${ARGN})
            SET(${testname}_SRC ${${testname}_SRC} ${src})
        endforeach(src)
        qt4_automoc(${testsrc})
        add_executable(${testname} ${${testname}_SRC})
        target_link_libraries(${testname} ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES} ${TEST_LIBRARIES})
        add_test(${testname} ${testname})
        add_custom_command(TARGET ${testname} POST_BUILD COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${testname})
    endif( ENABLE_TESTING )
ENDMACRO(add_qttest)

add_qttest(testenum testenum.cpp ${TEST_SOURCES})
add_qttest(testproperty testproperty.cpp ${TEST_SOURCES})
add_qttest(testpropertyset testpropertyset.cpp ${TEST_SOURCES})
add_qttest(testdeviceproperty testdeviceproperty.cpp ${TEST_SOURCES})
add_qttest(testdeviceprofile testdeviceprofile.cpp ${TEST_SOURCES})
add_qttest(testtabletprofile testtabletprofile.cpp ${TEST_SOURCES})
add_qttest(testdeviceprofileconfigadaptor testdeviceprofileconfigadaptor.cpp ${TEST_SOURCES})
add_qttest(testtabletprofileconfigadaptor testtabletprofileconfigadaptor.cpp ${TEST_SOURCES})
add_qttest(testprofilemanager testprofilemanager.cpp ${TEST_SOURCES})

#add_qttest(testtest testtest.cpp ${TEST_SOURCES})
