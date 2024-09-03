set(SOURCE_LIST
    olbContinuous2D.hh
    olbMixing2D.hh
    olbOoc2D.hh
)

set(HEADER_LIST
    olbContinuous2D.h
    olbMixing2D.h
    olbOoc2D.h
)

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
