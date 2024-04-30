set(SOURCE_LIST 
    Architecture.test.cpp
<<<<<<< HEAD
    BigDroplet.test.cpp
    Continuous.test.cpp
=======
    Mixing.test.cpp
    Results.test.cpp
>>>>>>> addMixing1D
)

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST})
target_link_libraries(${TARGET_NAME} PUBLIC lbmLib)
<<<<<<< HEAD
target_link_libraries(${TARGET_NAME} PUBLIC simLib)
=======
target_link_libraries(${TARGET_NAME} PUBLIC hybridLib)
>>>>>>> addMixing1D
