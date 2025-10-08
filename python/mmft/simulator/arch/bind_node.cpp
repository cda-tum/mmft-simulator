#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Node.hh"

namespace py = pybind11;

using T = double;

void bind_node(py::module_& m) {

	py::class_<arch::Node<T>, py::smart_holder>(m, "Node")
		.def("getId", &arch::Node<T>::getId, "Returns the Id of the node.")
		.def("setPosition", &arch::Node<T>::setPosition, "Set the position of the node.")
		.def("getPosition", &arch::Node<T>::getPosition, "Returns the position of the node.")
		.def("getPressure", &arch::Node<T>::getPressure, "Returns the pressure at the node.")
		.def("setSink", &arch::Node<T>::setSink, "Set the node as a sink node.")
		.def("getSink", &arch::Node<T>::getSink, "Returns true if the node is a sink node.")
		.def("setGround", &arch::Node<T>::setGround, "Set the node as a ground node.")
		.def("getGround", &arch::Node<T>::getGround, "Returns true if the node is a ground node.");

}