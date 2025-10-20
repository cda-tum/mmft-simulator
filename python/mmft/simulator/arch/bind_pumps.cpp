#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "architecture/entities/Edge.hh"
#include "architecture/entities/FlowRatePump.hh"
#include "architecture/entities/PressurePump.hh"

namespace py = pybind11;

using T = double;

void bind_pumps(py::module_& m) {

	py::class_<arch::FlowRatePump<T>, arch::Edge<T>, py::smart_holder>(m, "FlowRatePump")
		.def("setFlowRate", &arch::FlowRatePump<T>::setFlowRate, "Set the flow rate of the pump [m^3/s].");

	py::class_<arch::PressurePump<T>, arch::Edge<T>, py::smart_holder>(m, "PressurePump")
		.def("setPressure", &arch::PressurePump<T>::setPressure, "Set the pressure across the pump [Pa].");

}