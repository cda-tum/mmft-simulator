set(HEADER_LIST
    unistd.h
)

target_sources(${TARGET_NAME} PUBLIC ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})