add_definitions(-DTRANSLATION_DOMAIN=\"wacomtablet\")

set(dataengine_SRCS
    wacomtabletengine.cpp
    wacomtabletservice.cpp
    multidbuspendingcallwatcher.cpp
)

add_library(plasma_engine_wacomtablet MODULE ${dataengine_SRCS})
target_link_libraries(plasma_engine_wacomtablet
                      KF5::Plasma
                      Qt5::DBus
                      wacom_common
)

kcoreaddons_desktop_to_json(plasma_engine_wacomtablet plasma-dataengine-wacomtablet.desktop)

install(TARGETS plasma_engine_wacomtablet
        DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/dataengine
)
install(FILES plasma-dataengine-wacomtablet.desktop
        DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
)
install(FILES wacomtablet.operations
        DESTINATION ${PLASMA_DATA_INSTALL_DIR}/services
)
