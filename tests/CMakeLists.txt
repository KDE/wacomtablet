
## make sure QTTEST is available
find_package(Qt4 REQUIRED)
include( ${QT_USE_FILE} )
set(QT_USE_QTTEST TRUE)

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

## TESTSUITES
add_subdirectory(common)
add_subdirectory(kded)
