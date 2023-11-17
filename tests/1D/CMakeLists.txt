set(SOURCE_LIST 
    Architecture.test.cpp
    NodalAnalysis.test.cpp
    Results.test.cpp
)

target_sources(${TARGET_NAME} PRIVATE ${SOURCE_LIST})