set(SOURCE_LIST 
    Concentration.test.cpp
    Continuous.test.cpp
    Opening.test.cpp
)

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST})
target_link_libraries(${TARGET_NAME} PUBLIC lbmLib)