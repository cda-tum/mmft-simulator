# add pybind11 library
add_subdirectory("${PROJECT_SOURCE_DIR}/external/pybind11" "external/pybind11" EXCLUDE_FROM_ALL)

set(PYTHON_MODULE_NAME "pyhybridsim")
if(NOT TARGET ${PYTHON_MODULE_NAME})
	pybind11_add_module(${PYTHON_MODULE_NAME} bindings.cpp)
	target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE hybridLib)
	target_compile_definitions(${PYTHON_MODULE_NAME} PRIVATE VERSION_INFO=${HYBRIDSIM_VERSION_INFO})
endif()
