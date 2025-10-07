# add pybind11 library
add_subdirectory("${PROJECT_SOURCE_DIR}/external/pybind11" "external/pybind11" EXCLUDE_FROM_ALL)

set(PYTHON_MODULE_NAME "pysimulator")

if(NOT TARGET ${PYTHON_MODULE_NAME})
	pybind11_add_module(${PYTHON_MODULE_NAME} 

		# Architecture bindings
		arch/bind_channel.cpp
		arch/bind_edge.cpp
		arch/bind_enums.cpp
		arch/bind_membrane.cpp
		arch/bind_module.cpp
		arch/bind_network.cpp
		arch/bind_node.cpp
		arch/bind_opening.cpp
		arch/bind_pumps.cpp
		arch/bind_tank.cpp

		# Simulator bindings
		sim/bind_abstractContinuous.cpp
		sim/bind_abstractDroplet.cpp
		sim/bind_abstractMembrane.cpp
		sim/bind_abstractMixing.cpp
		sim/bind_droplet.cpp
		sim/bind_fluid.cpp
		sim/bind_hybridContinuous.cpp
		sim/bind_injections.cpp
		sim/bind_mixture.cpp
		sim/bind_results.cpp
		sim/bind_simulation.cpp
		sim/bind_specie.cpp
		
	)
	target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE simLib)
	target_compile_definitions(${PYTHON_MODULE_NAME} PRIVATE VERSION_INFO=${SIMULATOR_VERSION_INFO})
endif()
