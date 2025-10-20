set(SOURCE_LIST
    ChannelPosition.hh
)
    
set(HEADER_LIST
    ChannelPosition.h
    ModuleOpening.h
)

target_sources(${TARGET_NAME} PUBLIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC nlohmann_json::nlohmann_json)
