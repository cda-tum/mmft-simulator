#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "simulation/entities/Fluid.hh"

namespace py = pybind11;

using T = double;

void bind_fluid(py::module_& m) {

	py::class_<sim::Fluid<T>, py::smart_holder>(m, "Fluid")
		.def("getId", &sim::Fluid<T>::getId, "Returns the id of the fluid.")
		.def("setName", &sim::Fluid<T>::setName, "Sets the name of the fluid.")
		.def("getName", &sim::Fluid<T>::getName, "Returns the name of the fluid.")
		.def("setViscosity", &sim::Fluid<T>::setViscosity, "Sets the viscosity of the fluid.")
		.def("getViscosity", &sim::Fluid<T>::getViscosity, "Returns the viscosity of the fluid.")
		.def("setDensity", &sim::Fluid<T>::setDensity, "Sets the density of the fluid.")
		.def("getDensity", &sim::Fluid<T>::getDensity, "Returns the density of the fluid.");

}