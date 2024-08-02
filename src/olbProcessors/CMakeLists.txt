set(SOURCE_LIST
    navierStokesAdvectionDiffusionCouplingPostProcessor2D.hh
    saturatedFluxPostProcessor2D.hh
    setFunctionalRegularizedHeatFlux.hh
)

set(HEADER_LIST
    navierStokesAdvectionDiffusionCouplingPostProcessor2D.h
    saturatedFluxPostProcessor2D.h
    setFunctionalRegularizedHeatFlux.h
)

target_sources(${TARGET_NAME} PUBLIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC lbmLib)