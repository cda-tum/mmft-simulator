cmake_minimum_required(VERSION 3.21.0)

project(hybrid-simulator
    VERSION 0.1.0
    DESCRIPTION "Hybrid simulator for closed channel-based microfluidic devices."
    HOMEPAGE_URL "https://github.com/cda-tum/mmft-hybrid-simulator.git"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(PLATFORM_CPU_SISD)

# download external libraries
include(FetchContent)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.3.9
)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_Declare(
    lbm
    URL https://www.openlb.net/wp-content/uploads/2022/04/olb-1.5r0.tgz
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)
FetchContent_MakeAvailable(eigen json lbm)

add_library(lbmLib)

set(EXTERNAL_DIR ${lbm_SOURCE_DIR}/external)

set(EXTERNAL_HEADER_LIST
    ${EXTERNAL_DIR}/tinyxml/tinyxml.h
    ${EXTERNAL_DIR}/tinyxml/tinystr.h
)

set(EXTERNAL_SOURCE_LIST
    ${EXTERNAL_DIR}/tinyxml/tinyxml.cpp
    ${EXTERNAL_DIR}/tinyxml/tinystr.cpp
    ${EXTERNAL_DIR}/tinyxml/tinyxmlerror.cpp
    ${EXTERNAL_DIR}/tinyxml/tinyxmlparser.cpp
)

target_sources(lbmLib PUBLIC ${EXTERNAL_SOURCE_LIST} ${EXTERNAL_HEADER_LIST})

target_include_directories(
    lbmLib PUBLIC ${lbm_SOURCE_DIR}/src 
    ${lbm_SOURCE_DIR}/external/tinyxml
)
# add library
set(TARGET_NAME hybridLib)
add_library(${TARGET_NAME})

# add sources
add_subdirectory(src)

# create executable and tests (if build as main project)

# main executable
add_executable(hybridSim)
target_sources(hybridSim PUBLIC src/main.cpp)
target_link_libraries(hybridSim PUBLIC ${TARGET_NAME})

# Create python library
set(PY_TARGET_NAME mmft.hybridSim)

add_subdirectory(external/pybind11)
pybind11_add_module(
    ${PY_TARGET_NAME}
    python/mainPy.cpp
)

target_link_libraries(${PY_TARGET_NAME} PUBLIC hybridLib)

