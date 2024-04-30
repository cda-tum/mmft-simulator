<<<<<<< HEAD
add_subdirectory(1D)
add_subdirectory(hybrid)
=======
#add_subdirectory(1D)
#add_subdirectory(hybrid)
>>>>>>> addMixing1D

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST})
target_link_libraries(${TARGET_NAME} PUBLIC gtest gtest_main)
target_link_libraries(${TARGET_NAME} PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(${TARGET_NAME} PUBLIC lbmLib)
<<<<<<< HEAD
target_link_libraries(${TARGET_NAME} PUBLIC simLib)
=======
target_link_libraries(${TARGET_NAME} PUBLIC hybridLib)
>>>>>>> addMixing1D
