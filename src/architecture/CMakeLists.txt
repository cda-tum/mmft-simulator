set(SOURCE_LIST
    Channel.hh
    ChannelPosition.hh
    Edge.hh
    FlowRatePump.hh
    Network.hh
    Node.hh
    PressurePump.hh
    Pump.hh
)
    
set(HEADER_LIST
    Channel.h
    ChannelPosition.h
    Edge.h
    FlowRatePump.h
    Network.h
    Node.h
    Platform.h
    PressurePump.h
    Pump.h
)

# add subdirectories
add_subdirectory(modules)

target_sources(${TARGET_NAME} PUBLIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC nlohmann_json::nlohmann_json)