#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Channel.hh"
#include "architecture/entities/Edge.hh"
#include "architecture/entities/FlowRatePump.hh"
#include "architecture/entities/Membrane.hh"
#include "architecture/entities/Module.hh"
#include "architecture/entities/Node.hh"
#include "architecture/entities/PressurePump.hh"
#include "architecture/entities/Tank.hh"
#include "architecture/definitions/ModuleOpening.h"
#include "architecture/Network.hh"

namespace py = pybind11;

using T = double;

void bind_module(py::module_& m) {

	py::class_<arch::Module<T>, py::smart_holder>(m, "Module")
		.def("getId", &arch::Module<T>::getId, "Returns the id of the module.")
		.def("getPosition", &arch::Module<T>::getPosition, "Returns the position [x, y] of the bottom left corner w.r.t. the device [m].")
		.def("getSize", &arch::Module<T>::getSize, "Returns the size [x, y] of the module [m].")
		.def("getNodes", &arch::Module<T>::getNodes, "Returns a map of nodes that are on the module's edges.")
		.def("getModuleType", &arch::Module<T>::getModuleType, "Returns the type of the module.");

	py::class_<arch::CfdModule<T>, arch::Module<T>, py::smart_holder>(m, "CfdModule")
		.def("assertInitialized", &arch::CfdModule<T>::assertInitialized, "Checks whether the CFD module has valid openings and STL definitions.")
		.def("getNetwork", &arch::CfdModule<T>::getNetwork, "Returns a graph network that fully connects the module's boundary nodes.")
		.def("getOpenings", &arch::CfdModule<T>::getOpenings, "Returns a map of the openings of the module.")
		.def("getStlFile", &arch::CfdModule<T>::getStlFile, "Returns the STL file that defines the CFD domain.");

}