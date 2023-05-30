set(SOURCE_LIST
    CFDSim.hh
    Fluid.hh
    ResistanceModels.hh
    Simulation.hh
)

set(HEADER_LIST
    CFDSim.h
    Fluid.h
    ResistanceModels.h
    Simulation.h
)

target_sources(${TARGET_NAME} PUBLIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC lbmLib)