set(SOURCE_LIST
    navierStokesAdvectionDiffusionCouplingPostProcessor2D.hh
    navierStokesAdvectionDiffusionCouplingPostProcessor3D.hh
    saturatedFluxPostProcessor2D.hh
    setFunctionalRegularizedHeatFlux2D.hh
    setFunctionalRegularizedHeatFlux3D.hh
)

set(HEADER_LIST
    navierStokesAdvectionDiffusionCouplingPostProcessor2D.h
    navierStokesAdvectionDiffusionCouplingPostProcessor3D.h
    saturatedFluxPostProcessor2D.h
    setFunctionalRegularizedHeatFlux2D.h
    setFunctionalRegularizedHeatFlux3D.h
)

target_sources(${TARGET_NAME} PUBLIC ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC lbmLib)