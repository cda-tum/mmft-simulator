set(SOURCE_LIST
    Scheme.hh
    Naive.hh
)

set(HEADER_LIST
    Scheme.h
    Naive.h
)

target_sources(${TARGET_NAME} PUBLIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC eigen)