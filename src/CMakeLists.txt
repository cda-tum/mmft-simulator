set(SOURCE_LIST
    baseSimulator.hh
)

set(HEADER_LIST
    baseSimulator.h
)

# add subdirectories
add_subdirectory(nodalAnalysis)
add_subdirectory(architecture)
add_subdirectory(hybridDynamics)
add_subdirectory(simulation)
add_subdirectory(olbProcessors)
add_subdirectory(porting)
add_subdirectory(result)

target_sources(${TARGET_NAME} PUBLIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})