set(SOURCE_LIST
    ConcentrationSemantics.hh
)

set(HEADER_LIST
    ConcentrationSemantics.h
)

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})