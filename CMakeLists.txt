cmake_minimum_required(VERSION 3.31.6)

project(MMFT-simulator
    DESCRIPTION "Simulator for closed channel-based microfluidic devices."
    HOMEPAGE_URL "https://github.com/cda-tum/mmft-simulator.git"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(PLATFORM_CPU_SISD)
IF (NOT WIN32)
    set(CMAKE_CXX_FLAGS "-O3 -Wall -march=native -mtune=native -Wno-overloaded-virtual")
# ELSE()
#     set(CMAKE_CXX_FLAGS "-Wall -march=native /EHsc")
ENDIF()

# download external libraries
include(FetchContent)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_Declare(
    lbm
    GIT_REPOSITORY https://gitlab.com/openlb/release.git
    GIT_TAG 1.7.0
)
FetchContent_MakeAvailable(eigen googletest json lbm)

option(USE_ESSLBM "USE_ESSLBM" OFF)
if(USE_ESSLBM)
    ADD_DEFINITIONS(-DUSE_ESSLBM)
    find_package(ESSLbm REQUIRED)
    find_package(OpenMP REQUIRED)
    find_package(MPI REQUIRED)
    find_package(CUDAToolkit REQUIRED)
endif()

add_library(lbmLib)
target_compile_features(lbmLib PRIVATE cxx_std_17)

# if (WIN32)
#     add_subdirectory(incl/windows)
# endif()

set(LBM_SOURCE_LIST
    ${lbm_SOURCE_DIR}/external/tinyxml/tinyxml.cpp
    ${lbm_SOURCE_DIR}/external/tinyxml/tinystr.cpp
    ${lbm_SOURCE_DIR}/external/tinyxml/tinyxmlerror.cpp
    ${lbm_SOURCE_DIR}/external/tinyxml/tinyxmlparser.cpp
    ${lbm_SOURCE_DIR}/src/communication/mpiManager.cpp
    ${lbm_SOURCE_DIR}/src/io/ostreamManager.cpp
    ${lbm_SOURCE_DIR}/src/core/olbInit.cpp
)

option(VERBOSE "Verbose" ON)
IF(VERBOSE)
    ADD_DEFINITIONS(-DVERBOSE)
ENDIF()

target_sources(lbmLib PUBLIC ${LBM_HEADER_LIST} ${LBM_SOURCE_LIST})

target_include_directories(
    lbmLib PUBLIC ${lbm_SOURCE_DIR}/src
    incl
    ${lbm_SOURCE_DIR}/external/tinyxml
    ${lbm_SOURCE_DIR}/external/zlib
)
# add library
set(TARGET_NAME simLib)
add_library(${TARGET_NAME})

# add sources
add_subdirectory(src)
# if (WIN32)
#     add_subdirectory(incl/windows)
# endif()

if(USE_ESSLBM)
    target_link_libraries(${TARGET_NAME} PUBLIC ESS::ESSLbm)
endif()

# create executable and tests (if build as main project)

# main executable
add_executable(MMFTSim)
target_sources(MMFTSim PUBLIC src/main.cpp)
target_link_libraries(MMFTSim PUBLIC ${TARGET_NAME})

if(USE_ESSLBM)
    target_link_libraries(MMFTSim PUBLIC ESS::ESSLbm)
endif()

# add Python binding code
option(BINDINGS "Configure for building Python bindings")
if(BINDINGS)
	add_subdirectory(python/mmft/simulator)
endif()

# create tests
option(TEST "Configure for building test cases")
if(TEST)
    enable_testing()
    include(GoogleTest)
    set(TARGET_NAME testLib)
    add_library(${TARGET_NAME})
    add_subdirectory(tests)
    add_executable(simulatorTest)
    target_sources(simulatorTest PUBLIC tests/test.cpp)
    target_link_libraries(simulatorTest PUBLIC gtest gtest_main)
    target_link_libraries(simulatorTest PUBLIC gtest lbmLib)
    target_link_libraries(simulatorTest PUBLIC gtest simLib)
    target_link_libraries(simulatorTest PUBLIC gtest testLib)
    gtest_discover_tests(simulatorTest)
endif()

# create benchmarks
option(BENCHMARK "Configure for building benchmarks")
if(BENCHMARK)
    find_package(benchmark REQUIRED)
    set(TARGET_NAME simulatorBenchmark)
    add_executable(simulatorBenchmark)
    target_sources(simulatorBenchmark PUBLIC benchmarks/benchmark.cpp)
    target_link_libraries(simulatorBenchmark PUBLIC gtest gtest_main)
    target_link_libraries(simulatorBenchmark PUBLIC gtest lbmLib)
    target_link_libraries(simulatorBenchmark PUBLIC gtest simLib)
    target_link_libraries(simulatorBenchmark PUBLIC benchmark::benchmark)
endif()
