set(SOURCE_LIST
    cfdSimulator.hh
    olbContinuous.hh
    olbMixing.hh
    olbOoc.hh
)

set(HEADER_LIST
    cfdSimulator.h
    olbContinuous.h
    olbMixing.h
    olbOoc.h
)

if(USE_ESSLBM)
    target_sources(${TARGET_NAME} PUBLIC essContinuous.hh essContinuous.h)
    target_sources(${TARGET_NAME} PUBLIC essMixing.hh essMixing.h)
endif()

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})