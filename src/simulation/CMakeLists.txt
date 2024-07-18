set(SOURCE_LIST
    CFDSim.hh
    Droplet.hh
    Fluid.hh
    Injection.hh
    MixingModels.hh
    Mixture.hh
    MixtureInjection.hh
    ResistanceModels.hh
    Simulation.hh
    Specie.hh
    Tissue.hh
)

set(HEADER_LIST
    CFDSim.h
    Droplet.h
    Fluid.h
    Injection.h
    MixingModels.h
    Mixture.h
    MixtureInjection.h
    ResistanceModels.h
    Simulation.h
    Specie.h
    Tissue.h
)

target_sources(${TARGET_NAME} PUBLIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC lbmLib)

add_subdirectory(events)
add_subdirectory(simulators)