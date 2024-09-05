cmake_minimum_required(VERSION 3.21.0)

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
set(CMAKE_CXX_FLAGS "-O3 -Wall -march=native -mtune=native")

IF (WIN32)
    set(patch_file ${CMAKE_CURRENT_BINARY_DIR}/_deps/lbm-src/src/core/singleton.h)
    set(lbm_patch sed -i -f ${CMAKE_CURRENT_SOURCE_DIR}/script.sed ${patch_file})
ELSE()
    set(lbm_patch )
ENDIF()

# download external libraries
include(FetchContent)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.3.9
)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_Declare(
    lbm
    URL https://www.openlb.net/wp-content/uploads/2022/04/olb-1.5r0.tgz
    PATCH_COMMAND ${lbm_patch}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
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

set(EXTERNAL_DIR ${lbm_SOURCE_DIR}/external)

set(EXTERNAL_HEADER_LIST
    ${EXTERNAL_DIR}/tinyxml/tinyxml.h
    ${EXTERNAL_DIR}/tinyxml/tinystr.h
    ${EXTERNAL_DIR}/zlib/crc32.h
    ${EXTERNAL_DIR}/zlib/deflate.h
    ${EXTERNAL_DIR}/zlib/gzguts.h
    ${EXTERNAL_DIR}/zlib/inffast.h
    ${EXTERNAL_DIR}/zlib/inffixed.h
    ${EXTERNAL_DIR}/zlib/inflate.h
    ${EXTERNAL_DIR}/zlib/inftrees.h
    ${EXTERNAL_DIR}/zlib/trees.h
    ${EXTERNAL_DIR}/zlib/zconf.h
    ${EXTERNAL_DIR}/zlib/zlib.h
    ${EXTERNAL_DIR}/zlib/zutil.h
)

set(EXTERNAL_SOURCE_LIST
    ${EXTERNAL_DIR}/tinyxml/tinyxml.cpp
    ${EXTERNAL_DIR}/tinyxml/tinystr.cpp
    ${EXTERNAL_DIR}/tinyxml/tinyxmlerror.cpp
    ${EXTERNAL_DIR}/tinyxml/tinyxmlparser.cpp
    ${EXTERNAL_DIR}/zlib/adler32.c
    ${EXTERNAL_DIR}/zlib/compress.c
    ${EXTERNAL_DIR}/zlib/crc32.c
    ${EXTERNAL_DIR}/zlib/deflate.c
    ${EXTERNAL_DIR}/zlib/gzclose.c
    ${EXTERNAL_DIR}/zlib/gzlib.c
    ${EXTERNAL_DIR}/zlib/gzread.c
    ${EXTERNAL_DIR}/zlib/gzwrite.c
    ${EXTERNAL_DIR}/zlib/infback.c
    ${EXTERNAL_DIR}/zlib/inffast.c
    ${EXTERNAL_DIR}/zlib/inflate.c
    ${EXTERNAL_DIR}/zlib/inftrees.c
    ${EXTERNAL_DIR}/zlib/trees.c
    ${EXTERNAL_DIR}/zlib/uncompr.c
    ${EXTERNAL_DIR}/zlib/zutil.c
)

option(VERBOSE "Verbose" ON)
IF(VERBOSE)
    ADD_DEFINITIONS(-DVERBOSE)
ENDIF()

target_sources(lbmLib PUBLIC ${EXTERNAL_SOURCE_LIST} ${EXTERNAL_HEADER_LIST})

target_include_directories(
    lbmLib PUBLIC ${lbm_SOURCE_DIR}/src
    ${lbm_SOURCE_DIR}/external/tinyxml
    ${lbm_SOURCE_DIR}/external/zlib
)
# add library
set(TARGET_NAME simLib)
add_library(${TARGET_NAME})

# add sources
add_subdirectory(src)

if(USE_ESSLBM)
    target_link_libraries(${TARGET_NAME} PUBLIC ESS::ESSLbm)
endif()

# create executable and tests (if build as main project)

#set (DIM "BOTH")
#IF(2D)
#    set(DIM "2D")
#ELSEIF(3D)
#    set(DIM "3D")
#ENDIF()

#IF((DIM EQUAL "2D") OR (DIM EQUAL "BOTH"))

# main executable
add_executable(MMFTSim2D)
target_compile_definitions(MMFTSim2D PUBLIC DIMENSION=2)
target_sources(MMFTSim2D PUBLIC src/main.cpp)
target_link_libraries(MMFTSim2D PUBLIC ${TARGET_NAME})

#ENDIF()

#IF((DIM EQUAL "3D") OR (DIM EQUAL "BOTH"))

# main executable
add_executable(MMFTSim3D)
target_compile_definitions(MMFTSim3D PUBLIC DIMENSION=3)
target_sources(MMFTSim3D PUBLIC src/main.cpp)
target_link_libraries(MMFTSim3D PUBLIC ${TARGET_NAME})

#ENDIF()

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
