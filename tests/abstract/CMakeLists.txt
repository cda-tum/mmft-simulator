set(SOURCE_LIST 
    Architecture.test.cpp
    Concentration.test.cpp
    Continuous.test.cpp
    Droplet.test.cpp
    GradientGenerator.test.cpp
    Membrane.test.cpp
    Topology.test.cpp
)

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST})
target_link_libraries(${TARGET_NAME} PUBLIC lbmLib)
target_link_libraries(${TARGET_NAME} PUBLIC simLib)