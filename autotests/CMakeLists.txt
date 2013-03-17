
if(KDE4_BUILD_TESTS)
    include_directories( ${CMAKE_SOURCE_DIR}/src )

    # Add Tests
    add_subdirectory( common/buttonshortcut )
    add_subdirectory( common/deviceinformation )
    add_subdirectory( common/deviceprofile )
    add_subdirectory( common/deviceprofileconfigadaptor )
    add_subdirectory( common/deviceproperty )
    add_subdirectory( common/enum )
    add_subdirectory( common/profilemanager )
    add_subdirectory( common/property )
    add_subdirectory( common/propertyset )
    add_subdirectory( common/tabletinformation )
    add_subdirectory( common/tabletprofile )
    add_subdirectory( common/tabletprofileconfigadaptor )

endif()
