set(SOURCE_LIST
    Simulation.hh
    AbstractContinuous.hh
    AbstractDroplet.hh
    AbstractMixing.hh
    AbstractMembrane.hh
    CFDSim.hh
    HybridContinuous.hh
    HybridMixing.hh
)

set(HEADER_LIST
    Simulation.h
    AbstractContinuous.h
    AbstractDroplet.h
    AbstractMixing.h
    AbstractMembrane.h
    CFDSim.h
    HybridContinuous.h
    HybridMixing.h
)

target_sources(${TARGET_NAME} PUBLIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC lbmLib)

add_subdirectory(cfdHandlers)
add_subdirectory(semantics)