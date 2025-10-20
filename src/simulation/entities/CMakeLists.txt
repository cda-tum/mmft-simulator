set(SOURCE_LIST
    Droplet.hh
    Fluid.hh
    Mixture.hh
    Specie.hh
    Tissue.hh
)

set(HEADER_LIST
    Droplet.h
    Fluid.h
    Mixture.h
    Specie.h
    Tissue.h
)

target_sources(${TARGET_NAME} PUBLIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC lbmLib)