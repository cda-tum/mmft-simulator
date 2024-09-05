set(SOURCE_LIST
    cfdSimulator.hh
)

set(HEADER_LIST
    cfdSimulator.h
    olbSim.h
)

if(USE_ESSLBM)
    target_sources(${TARGET_NAME} PUBLIC essContinuous.hh essContinuous.h)
    target_sources(${TARGET_NAME} PUBLIC essMixing.hh essMixing.h)
endif()

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

#if ((DIM equal "2D") or (DIM equal "BOTH"))
    add_subdirectory(2D)
#endif()
#if ((DIM equal "3D") or (DIM equal "BOTH"))
    add_subdirectory(3D)
#endif()