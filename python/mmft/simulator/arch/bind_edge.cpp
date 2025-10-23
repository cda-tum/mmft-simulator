#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Edge.hh"

namespace py = pybind11;

using T = double;

void bind_edge(py::module_& m) {

	py::class_<arch::Edge<T>, py::smart_holder>(m, "Edge")
		.def("getId", &arch::Edge<T>::getId, "Returns the Id of the edge.")
		.def("getNodeAId", &arch::Edge<T>::getNodeAId, "Returns the id of the node at the start of the edge.")
		.def("getNodeBId", &arch::Edge<T>::getNodeBId, "Returns the id of the node at the end of the edge.")
		.def("getNodeIds", &arch::Edge<T>::getNodeIds, "Returns the ids of the edge's nodes.")
		.def("getPressure", &arch::Edge<T>::getPressure, "Returns the pressure of the edge [Pa].")
		.def("getFlowRate", &arch::Edge<T>::getFlowRate, "Returns the flow rate within the edge [m^3/s].")
		.def("getResistance", &arch::Edge<T>::getResistance, "Returns the inherent resistance of the edge [Pa s].");

}