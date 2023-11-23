add_definitions(-DTRANSLATION_DOMAIN=\"wacomtablet\")

set(dataengine_SRCS
    wacomtabletengine.cpp
    wacomtabletservice.cpp
    multidbuspendingcallwatcher.cpp

    wacomtabletengine.h
    wacomtabletservice.h
    multidbuspendingcallwatcher.h
)

add_library(plasma_engine_wacomtablet MODULE ${dataengine_SRCS})
target_link_libraries(plasma_engine_wacomtablet
                      Plasma::Plasma
                      Qt::DBus
                      wacom_common
)

target_link_libraries(plasma_engine_wacomtablet KF6::Plasma5Support)

install(TARGETS plasma_engine_wacomtablet
        DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/dataengine
)
install(FILES wacomtablet.operations
        DESTINATION ${PLASMA_DATA_INSTALL_DIR}/services
)
