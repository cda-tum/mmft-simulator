set(SOURCE_LIST
    olbContinuous3D.hh
    olbMixing3D.hh
    olbOoc3D.hh
)

set(HEADER_LIST
    3D.h
    olbContinuous3D.h
    olbMixing3D.h
    olbOoc3D.h
)

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
