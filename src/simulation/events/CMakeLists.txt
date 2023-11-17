set(SOURCE_LIST
    BoundaryEvent.hh
    InjectionEvent.hh
    MergingEvent.hh
)

set(HEADER_LIST
    BoundaryEvent.h
    Event.h
    InjectionEvent.h
    MergingEvent.h
)

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})