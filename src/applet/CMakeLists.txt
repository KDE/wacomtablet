project(plasma-wacomtabletsettings)

include_directories(
                    ${KDE4_INCLUDES}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(tabletsettings_SRCS
    wacomtabletsettings.cpp
    tabletapplet.cpp)

add_library(plasma_applet_wacomtabletsettings ${tabletsettings_SRCS})
target_link_libraries(plasma_applet_wacomtabletsettings wacom_common ${KDE4_PLASMA_LIBS} ${KDE4_KUTILS_LIBRARY})

install(TARGETS plasma_applet_wacomtabletsettings DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-wacomtabletsettings.desktop DESTINATION ${SERVICES_INSTALL_DIR})
